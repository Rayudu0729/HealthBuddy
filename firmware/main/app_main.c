#include "esp_log.h"
#include "esp_ota_ops.h"

#include "binduos.h"
#include "i2c_bus.h"
#include "feedback.h"
#include "buttons.h"
#include "sensors.h"
#include "fall_detect.h"
#include "voice.h"
#include "alerts.h"
#include "sync.h"
#include "power.h"

static const char *TAG = "main";

void app_main(void)
{
    ESP_ERROR_CHECK(bindu_init());

    // Shared I2C bus first; a failure only disables MPU/haptics, buttons + buzzer still work.
    esp_err_t err = i2c_bus_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C bus failed (%s) - fall detection and haptics unavailable", esp_err_to_name(err));
    }

    // Safety-critical modules first, so they are subscribed before anything publishes.
    ESP_ERROR_CHECK(feedback_init());
    ESP_ERROR_CHECK(alerts_init());
    ESP_ERROR_CHECK(fall_detect_init());
    ESP_ERROR_CHECK(buttons_init());
    ESP_ERROR_CHECK(sensors_init());
    ESP_ERROR_CHECK(power_init());
    ESP_ERROR_CHECK(voice_init());
    ESP_ERROR_CHECK(sync_init());

    // Fail-safe OTA: reaching here means this firmware boots fine, so keep it.
    // If a new image crashes before this line, the bootloader rolls back.
    esp_ota_mark_app_valid_cancel_rollback();

    ESP_LOGI(TAG, "Bindu ready");
    feedback_play(FEEDBACK_BOOT_READY);
}
