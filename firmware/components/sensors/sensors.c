// Vitals and GPS. Read rates follow the power mode.

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "saathios.h"
#include "sensors.h"

static const char *TAG = "sensors";

static void sensors_task(void *arg)
{
    for (;;) {
        saathi_mode_t mode = saathi_get_mode();

        if (mode != SAATHI_MODE_SAFETY) {
            // TODO (Member 2): MAX30102 heart rate + SpO2 over I2C.
            // TODO (Member 2): MLX90614 temperature (optional).
            ESP_LOGD(TAG, "vitals read (stub)");
        }
        if (mode == SAATHI_MODE_ALERT) {
            // TODO (Member 2): NEO-6M GPS fix over UART; share with alerts.
            ESP_LOGD(TAG, "gps read (stub)");
        }

        int period_ms = (mode == SAATHI_MODE_NIGHT) ? 60000 : 5000;
        vTaskDelay(pdMS_TO_TICKS(period_ms));
    }
}

esp_err_t sensors_init(void)
{
    BaseType_t ok = xTaskCreatePinnedToCore(sensors_task, "sensors", 4096, NULL,
                                            SAATHI_PRIO_VITALS, NULL, SAATHI_CORE_NET);
    return ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}
