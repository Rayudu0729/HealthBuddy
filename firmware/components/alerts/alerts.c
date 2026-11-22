// Alert flow:
//   FALL -> ALERT mode, 10 s cancel window with a countdown tick each second.
//           CANCEL inside the window = nothing sent; window expiry = send.
//   SOS (already held 1.5 s by the buttons module) -> confirm pattern, send now.
// Sending is a stub for now: no network yet.

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "binduos.h"
#include "feedback.h"
#include "alerts.h"

static const char *TAG = "alerts";

#define FALL_CANCEL_WINDOW_S 10
#define TICK_PERIOD_MS       1000

static QueueHandle_t s_alert_queue;

static void on_event(const bindu_event_t *evt, void *ctx)
{
    xQueueSend(s_alert_queue, evt, 0);
}

// TODO: HTTPS event to the backend -> httpSMS Android gateway (no credentials in firmware).
static void send_alert(const char *what)
{
    ESP_LOGW(TAG, "ALERT SENT (stub): %s", what);
    feedback_play(FEEDBACK_ALERT_SENT);
}

static void alert_task(void *arg)
{
    bindu_event_t evt;
    char msg[48];
    int countdown_s = 0;          // 0 = no fall window running
    float pending_fall_g = 0.0f;
    TickType_t next_tick = 0;

    for (;;) {
        TickType_t wait = portMAX_DELAY;
        if (countdown_s > 0) {
            TickType_t now = xTaskGetTickCount();
            wait = ((int32_t)(next_tick - now) > 0) ? (next_tick - now) : 0;
        }

        if (xQueueReceive(s_alert_queue, &evt, wait) != pdTRUE) {
            // Timeout: one second of the cancel window has passed.
            if (countdown_s > 0) {
                countdown_s--;
                next_tick += pdMS_TO_TICKS(TICK_PERIOD_MS);
                if (countdown_s == 0) {
                    snprintf(msg, sizeof(msg), "FALL %.1f g", pending_fall_g);
                    send_alert(msg);
                } else {
                    ESP_LOGW(TAG, "Fall alert in %d s - press CANCEL", countdown_s);
                    feedback_play(FEEDBACK_FALL_COUNTDOWN_TICK);
                }
            }
            continue;
        }

        switch (evt.id) {
        case BINDU_EVT_FALL:
            if (countdown_s > 0) {
                ESP_LOGI(TAG, "FALL ignored - cancel window already running");
                break;
            }
            bindu_set_mode(BINDU_MODE_ALERT);
            pending_fall_g = evt.value;
            countdown_s = FALL_CANCEL_WINDOW_S;
            ESP_LOGW(TAG, "Fall alert in %d s - press CANCEL", countdown_s);
            feedback_play(FEEDBACK_FALL_COUNTDOWN_TICK);
            next_tick = xTaskGetTickCount() + pdMS_TO_TICKS(TICK_PERIOD_MS);
            break;
        case BINDU_EVT_CANCEL:
            if (countdown_s > 0) {
                countdown_s = 0;
                ESP_LOGW(TAG, "Fall alert CANCELLED by user - nothing sent");
                feedback_play(FEEDBACK_ALERT_CANCELLED);
                bindu_set_mode(BINDU_MODE_DAY);
            } else {
                ESP_LOGD(TAG, "CANCEL ignored - no alert pending");
            }
            break;
        case BINDU_EVT_SOS_BUTTON:
            if (countdown_s > 0) {
                ESP_LOGW(TAG, "SOS overrides pending fall countdown");
                countdown_s = 0;
            }
            bindu_set_mode(BINDU_MODE_ALERT);
            feedback_play(FEEDBACK_SOS_CONFIRMED);
            send_alert("SOS");
            break;
        case BINDU_EVT_MISSED_CALL:
            ESP_LOGI(TAG, "NOTIFY (stub): missed call, location and vitals TODO");
            break;
        case BINDU_EVT_LOW_BATTERY:
            ESP_LOGI(TAG, "NOTIFY (stub): battery low (%.0f%%)", evt.value);
            break;
        default:
            break;
        }
    }
}

esp_err_t alerts_init(void)
{
    s_alert_queue = xQueueCreate(8, sizeof(bindu_event_t));
    if (s_alert_queue == NULL) {
        return ESP_ERR_NO_MEM;
    }

    bindu_subscribe(BINDU_EVT_FALL, on_event, NULL);
    bindu_subscribe(BINDU_EVT_SOS_BUTTON, on_event, NULL);
    bindu_subscribe(BINDU_EVT_CANCEL, on_event, NULL);
    bindu_subscribe(BINDU_EVT_MISSED_CALL, on_event, NULL);
    bindu_subscribe(BINDU_EVT_LOW_BATTERY, on_event, NULL);

    BaseType_t ok = xTaskCreatePinnedToCore(alert_task, "alerts", 4096, NULL,
                                            BINDU_PRIO_SOS, NULL, BINDU_CORE_NET);
    return ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}
