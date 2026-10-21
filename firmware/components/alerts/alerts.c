// Alerts: event handlers only queue work; the alert task does the slow GSM part.
// The GSM mutex makes sure an SMS and a call never use the modem at the same time.

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "binduos.h"
#include "alerts.h"

static const char *TAG = "alerts";
static QueueHandle_t s_alert_queue;
static SemaphoreHandle_t s_gsm_lock;

static void on_event(const bindu_event_t *evt, void *ctx)
{
    if (evt->id == BINDU_EVT_FALL || evt->id == BINDU_EVT_SOS_BUTTON) {
        bindu_set_mode(BINDU_MODE_ALERT);
    }
    xQueueSend(s_alert_queue, evt, 0);
}

// TODO (Member 1 + Member 2): AT commands to the A7670C (4G) over UART.
static void gsm_send_sms(const char *text)
{
    xSemaphoreTake(s_gsm_lock, portMAX_DELAY);
    ESP_LOGW(TAG, "SMS (stub): %s", text);
    xSemaphoreGive(s_gsm_lock);
}

static void alert_task(void *arg)
{
    bindu_event_t evt;
    char msg[96];

    for (;;) {
        if (xQueueReceive(s_alert_queue, &evt, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        switch (evt.id) {
        case BINDU_EVT_FALL:
            // TODO: 10-second cancel window + buzzer before sending.
            snprintf(msg, sizeof(msg), "Bindu: fall detected (%.1f g). Location: TODO", evt.value);
            gsm_send_sms(msg);
            break;
        case BINDU_EVT_SOS_BUTTON:
            gsm_send_sms("Bindu: SOS pressed. Location: TODO");
            break;
        case BINDU_EVT_MISSED_CALL:
            gsm_send_sms("Bindu: location and vitals: TODO");
            break;
        case BINDU_EVT_LOW_BATTERY:
            snprintf(msg, sizeof(msg), "Bindu: battery low (%.0f%%), please charge", evt.value);
            gsm_send_sms(msg);
            break;
        default:
            break;
        }
    }
}

esp_err_t alerts_init(void)
{
    s_alert_queue = xQueueCreate(8, sizeof(bindu_event_t));
    s_gsm_lock = xSemaphoreCreateMutex();
    if (s_alert_queue == NULL || s_gsm_lock == NULL) {
        return ESP_ERR_NO_MEM;
    }

    bindu_subscribe(BINDU_EVT_FALL, on_event, NULL);
    bindu_subscribe(BINDU_EVT_SOS_BUTTON, on_event, NULL);
    bindu_subscribe(BINDU_EVT_MISSED_CALL, on_event, NULL);
    bindu_subscribe(BINDU_EVT_LOW_BATTERY, on_event, NULL);

    BaseType_t ok = xTaskCreatePinnedToCore(alert_task, "alerts", 4096, NULL,
                                            BINDU_PRIO_SOS, NULL, BINDU_CORE_NET);
    return ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}
