// Fall detection: an impact spike followed by stillness = FALL.
// A rise from low to normal posture with movement = STAND_UP (feeds the stand-up guard).
// Thresholds are starting guesses — tune them on recorded data in data/.

#include <math.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "binduos.h"
#include "fall_detect.h"
#include "mpu6050.h"

static const char *TAG = "fall_detect";

#define SAMPLE_PERIOD_MS     10      // 100 Hz
#define IMPACT_G             2.5f    // TODO tune
#define STILL_TOLERANCE_G    0.15f   // |a| within 1 g +/- this counts as still
#define STILL_TIME_MS        2000    // stillness needed after impact
#define ERR_LOG_PERIOD_US    5000000 // rate-limit for read-error logs
#define MAG_LOG_PERIOD_MS    1000    // demo log of |a| while waiting for stillness

static void fall_task(void *arg)
{
    bool impact_seen = false;
    float impact_g = 0.0f;
    int still_ms = 0;
    int mag_log_ms = 0;
    int64_t last_err_log_us = -ERR_LOG_PERIOD_US;
    TickType_t last_wake = xTaskGetTickCount();

    for (;;) {
        float ax, ay, az;
        esp_err_t err = mpu6050_read_accel_g(&ax, &ay, &az);
        if (err != ESP_OK) {
            // No fake data: skip this sample, keep state, log at most every 5 s.
            int64_t now = esp_timer_get_time();
            if (now - last_err_log_us >= ERR_LOG_PERIOD_US) {
                ESP_LOGE(TAG, "accel read failed: %s (sample skipped)", esp_err_to_name(err));
                last_err_log_us = now;
            }
            vTaskDelay(pdMS_TO_TICKS(SAMPLE_PERIOD_MS));
            last_wake = xTaskGetTickCount();
            continue;
        }
        float mag = sqrtf(ax * ax + ay * ay + az * az);

        if (impact_seen) {
            mag_log_ms += SAMPLE_PERIOD_MS;
            if (mag_log_ms >= MAG_LOG_PERIOD_MS) {
                mag_log_ms = 0;
                ESP_LOGI(TAG, "|a| = %.2f g", mag);
            }
        }

        if (mag > IMPACT_G) {
            if (!impact_seen) {
                ESP_LOGI(TAG, "impact %.2f g - waiting for %d ms stillness", mag, STILL_TIME_MS);
                mag_log_ms = 0;
                impact_g = mag;
            } else {
                impact_g = fmaxf(impact_g, mag);
            }
            impact_seen = true;
            still_ms = 0;
        } else if (impact_seen) {
            if (fabsf(mag - 1.0f) < STILL_TOLERANCE_G) {
                still_ms += SAMPLE_PERIOD_MS;
                if (still_ms >= STILL_TIME_MS) {
                    ESP_LOGW(TAG, "fall detected, impact %.1f g", impact_g);
                    bindu_publish(BINDU_EVT_FALL, impact_g);
                    impact_seen = false;
                }
            } else {
                still_ms = 0;   // moving again: probably not a fall
            }
        }

        // TODO (Lead): stand-up detection from the gyroscope + posture change,
        // then bindu_publish(BINDU_EVT_STAND_UP, 0).

        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(SAMPLE_PERIOD_MS));
    }
}

esp_err_t fall_detect_init(void)
{
    esp_err_t err = mpu6050_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "MPU-6050 not found at 0x68 (%s) - fall detection DISABLED, SOS still works",
                 esp_err_to_name(err));
        return ESP_OK;
    }
    BaseType_t ok = xTaskCreatePinnedToCore(fall_task, "fall_detect", 4096, NULL,
                                            BINDU_PRIO_FALL, NULL, BINDU_CORE_SAFETY);
    return ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}
