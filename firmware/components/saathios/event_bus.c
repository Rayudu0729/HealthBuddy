// Event bus: modules publish events; a dispatcher task on the safety core
// calls every subscriber. Handlers must be short — hand heavy work to your own task.

#include <stdbool.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "saathios.h"

#define MAX_SUBSCRIBERS 24
#define QUEUE_LEN       32

static const char *TAG = "saathi_bus";

typedef struct {
    saathi_event_id_t id;
    saathi_event_handler_t handler;
    void *ctx;
} subscriber_t;

static subscriber_t s_subs[MAX_SUBSCRIBERS];
static int s_sub_count;
static SemaphoreHandle_t s_subs_lock;
static QueueHandle_t s_queue;

static const char *const EVENT_NAMES[SAATHI_EVT_COUNT] = {
    [SAATHI_EVT_FALL]        = "FALL",
    [SAATHI_EVT_STAND_UP]    = "STAND_UP",
    [SAATHI_EVT_RISK_HIGH]   = "RISK_HIGH",
    [SAATHI_EVT_WAKE_WORD]   = "WAKE_WORD",
    [SAATHI_EVT_SOS_BUTTON]  = "SOS_BUTTON",
    [SAATHI_EVT_MISSED_CALL] = "MISSED_CALL",
    [SAATHI_EVT_LOW_BATTERY] = "LOW_BATTERY",
};

const char *saathi_event_name(saathi_event_id_t id)
{
    return (id < SAATHI_EVT_COUNT) ? EVENT_NAMES[id] : "UNKNOWN";
}

static void dispatcher_task(void *arg)
{
    saathi_event_t evt;
    subscriber_t snapshot[MAX_SUBSCRIBERS];

    for (;;) {
        if (xQueueReceive(s_queue, &evt, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        ESP_LOGI(TAG, "event %s value=%.2f", saathi_event_name(evt.id), evt.value);

        // Copy the table so handlers can run without holding the lock.
        xSemaphoreTake(s_subs_lock, portMAX_DELAY);
        int count = s_sub_count;
        memcpy(snapshot, s_subs, sizeof(subscriber_t) * count);
        xSemaphoreGive(s_subs_lock);

        for (int i = 0; i < count; i++) {
            if (snapshot[i].id == evt.id) {
                snapshot[i].handler(&evt, snapshot[i].ctx);
            }
        }
    }
}

esp_err_t saathi_init(void)
{
    s_subs_lock = xSemaphoreCreateMutex();
    s_queue = xQueueCreate(QUEUE_LEN, sizeof(saathi_event_t));
    if (s_subs_lock == NULL || s_queue == NULL) {
        return ESP_ERR_NO_MEM;
    }

    BaseType_t ok = xTaskCreatePinnedToCore(dispatcher_task, "saathi_bus", 4096, NULL,
                                            SAATHI_PRIO_BUS, NULL, SAATHI_CORE_SAFETY);
    if (ok != pdPASS) {
        return ESP_ERR_NO_MEM;
    }

    saathi_set_mode(SAATHI_MODE_DAY);
    ESP_LOGI(TAG, "SaathiOS started");
    return ESP_OK;
}

esp_err_t saathi_subscribe(saathi_event_id_t id, saathi_event_handler_t handler, void *ctx)
{
    if (id >= SAATHI_EVT_COUNT || handler == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = ESP_OK;
    xSemaphoreTake(s_subs_lock, portMAX_DELAY);
    if (s_sub_count < MAX_SUBSCRIBERS) {
        s_subs[s_sub_count++] = (subscriber_t){ .id = id, .handler = handler, .ctx = ctx };
    } else {
        err = ESP_ERR_NO_MEM;
    }
    xSemaphoreGive(s_subs_lock);
    return err;
}

esp_err_t saathi_publish(saathi_event_id_t id, float value)
{
    if (id >= SAATHI_EVT_COUNT) {
        return ESP_ERR_INVALID_ARG;
    }

    saathi_event_t evt = {
        .id = id,
        .timestamp_us = esp_timer_get_time(),
        .value = value,
    };

    // Safety events jump the queue so a FALL is never stuck behind routine events.
    bool urgent = (id == SAATHI_EVT_FALL || id == SAATHI_EVT_SOS_BUTTON);
    BaseType_t ok = urgent ? xQueueSendToFront(s_queue, &evt, 0)
                           : xQueueSendToBack(s_queue, &evt, 0);
    if (ok != pdTRUE) {
        ESP_LOGW(TAG, "queue full, dropped %s", saathi_event_name(id));
        return ESP_ERR_TIMEOUT;
    }
    return ESP_OK;
}
