#include "esp_log.h"
#include "esp_ota_ops.h"

#include "saathios.h"
#include "sensors.h"
#include "fall_detect.h"
#include "voice.h"
#include "alerts.h"
#include "sync.h"
#include "power.h"

static const char *TAG = "main";

void app_main(void)
{
    ESP_ERROR_CHECK(saathi_init());

    // Safety-critical modules first, so they are subscribed before anything publishes.
    ESP_ERROR_CHECK(alerts_init());
    ESP_ERROR_CHECK(fall_detect_init());
    ESP_ERROR_CHECK(sensors_init());
    ESP_ERROR_CHECK(power_init());
    ESP_ERROR_CHECK(voice_init());
    ESP_ERROR_CHECK(sync_init());

    // Fail-safe OTA: reaching here means this firmware boots fine, so keep it.
    // If a new image crashes before this line, the bootloader rolls back.
    esp_ota_mark_app_valid_cancel_rollback();

    ESP_LOGI(TAG, "Saathi ready");
}
