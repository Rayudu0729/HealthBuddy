// Alerts: event handlers only queue work; the alert task does the slow GSM part.
// The GSM mutex makes sure an SMS and a call never use the modem at the same time.

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "saathios.h"
#include "alerts.h"

static const char *TAG = "alerts";
static QueueHandle_t s_alert_queue;
static SemaphoreHandle_t s_gsm_lock;

static void on_event(const saathi_event_t *evt, void *ctx)
{
    if (evt->id == SAATHI_EVT_FALL || evt->id == SAATHI_EVT_SOS_BUTTON) {
        saathi_set_mode(SAATHI_MODE_ALERT);
    }
    xQueueSend(s_alert_queue, evt, 0);
}

// TODO (Member 1 + Member 2): AT commands to the SIM800L/A7670 over UART.
static void gsm_send_sms(const char *text)
{
    xSemaphoreTake(s_gsm_lock, portMAX_DELAY);
    ESP_LOGW(TAG, "SMS (stub): %s", text);
    xSemaphoreGive(s_gsm_lock);
}

static void alert_task(void *arg)
{
    saathi_event_t evt;
    char msg[96];

    for (;;) {
        if (xQueueReceive(s_alert_queue, &evt, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        switch (evt.id) {
        case SAATHI_EVT_FALL:
            // TODO: 10-second cancel window + buzzer before sending.
            snprintf(msg, sizeof(msg), "Saathi: fall detected (%.1f g). Location: TODO", evt.value);
            gsm_send_sms(msg);
            break;
        case SAATHI_EVT_SOS_BUTTON:
            gsm_send_sms("Saathi: SOS pressed. Location: TODO");
            break;
        case SAATHI_EVT_MISSED_CALL:
            gsm_send_sms("Saathi: location and vitals: TODO");
            break;
        case SAATHI_EVT_LOW_BATTERY:
            snprintf(msg, sizeof(msg), "Saathi: battery low (%.0f%%), please charge", evt.value);
            gsm_send_sms(msg);
            break;
        default:
            break;
        }
    }
}

esp_err_t alerts_init(void)
{
    s_alert_queue = xQueueCreate(8, sizeof(saathi_event_t));
    s_gsm_lock = xSemaphoreCreateMutex();
    if (s_alert_queue == NULL || s_gsm_lock == NULL) {
        return ESP_ERR_NO_MEM;
    }

    saathi_subscribe(SAATHI_EVT_FALL, on_event, NULL);
    saathi_subscribe(SAATHI_EVT_SOS_BUTTON, on_event, NULL);
    saathi_subscribe(SAATHI_EVT_MISSED_CALL, on_event, NULL);
    saathi_subscribe(SAATHI_EVT_LOW_BATTERY, on_event, NULL);

    BaseType_t ok = xTaskCreatePinnedToCore(alert_task, "alerts", 4096, NULL,
                                            SAATHI_PRIO_SOS, NULL, SAATHI_CORE_NET);
    return ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}
