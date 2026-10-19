// Uploads readings over Wi-Fi. Readings are kept in the storage partition
// when there is no Wi-Fi and sent later (offline queue).

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "saathios.h"
#include "sync.h"

static const char *TAG = "sync";

static void sync_task(void *arg)
{
    for (;;) {
        if (saathi_get_mode() != SAATHI_MODE_SAFETY) {
            // TODO (Member 1): Wi-Fi connect, publish JSON to MQTT or Firebase.
            // TODO (Member 1): flush the offline queue from the storage partition.
            ESP_LOGD(TAG, "sync (stub)");
        }
        vTaskDelay(pdMS_TO_TICKS(30000));
    }
}

esp_err_t sync_init(void)
{
    BaseType_t ok = xTaskCreatePinnedToCore(sync_task, "sync", 6144, NULL,
                                            SAATHI_PRIO_SYNC, NULL, SAATHI_CORE_NET);
    return ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}
