// Battery monitor and mode switching: DAY / NIGHT / SAFETY.
// ALERT is set by the alerts module and cleared here once things are calm.

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "binduos.h"
#include "power.h"

static const char *TAG = "power";

#define LOW_BATTERY_PCT 15

// TODO (Member 2): read battery voltage via ADC and map to percent.
static int read_battery_pct(void)
{
    return 100;
}

// TODO (Member 2): use RTC time (synced over Wi-Fi) to decide sleeping hours.
static bool is_night(void)
{
    return false;
}

static void power_task(void *arg)
{
    bool low_reported = false;

    for (;;) {
        int pct = read_battery_pct();

        if (pct <= LOW_BATTERY_PCT) {
            bindu_set_mode(BINDU_MODE_SAFETY);
            if (!low_reported) {
                bindu_publish(BINDU_EVT_LOW_BATTERY, (float)pct);
                low_reported = true;
            }
        } else {
            low_reported = false;
            // TODO: leave ALERT only after the caregiver acknowledges.
            if (bindu_get_mode() != BINDU_MODE_ALERT) {
                bindu_set_mode(is_night() ? BINDU_MODE_NIGHT : BINDU_MODE_DAY);
            }
        }

        ESP_LOGD(TAG, "battery %d%%, mode %s", pct, bindu_mode_name(bindu_get_mode()));
        vTaskDelay(pdMS_TO_TICKS(60000));
    }
}

esp_err_t power_init(void)
{
    // TODO (Member 2): esp_pm_configure() with light sleep, and the MPU6050
    // motion interrupt as a wake source for NIGHT mode.
    BaseType_t ok = xTaskCreatePinnedToCore(power_task, "power", 3072, NULL,
                                            BINDU_PRIO_POWER, NULL, BINDU_CORE_NET);
    return ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}
