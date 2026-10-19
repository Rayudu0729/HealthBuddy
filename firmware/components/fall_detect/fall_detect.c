// Fall detection: an impact spike followed by stillness = FALL.
// A rise from low to normal posture with movement = STAND_UP (feeds the stand-up guard).
// Thresholds are starting guesses — tune them on recorded data in data/.

#include <math.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "saathios.h"
#include "fall_detect.h"

static const char *TAG = "fall_detect";

#define SAMPLE_PERIOD_MS     10      // 100 Hz
#define IMPACT_G             2.5f    // TODO tune
#define STILL_TOLERANCE_G    0.15f   // |a| within 1 g +/- this counts as still
#define STILL_TIME_MS        2000    // stillness needed after impact

typedef struct { float x, y, z; } accel_t;

// TODO (Member 2): read the MPU6050 over I2C. Returns acceleration in g.
static accel_t read_accel(void)
{
    return (accel_t){ 0.0f, 0.0f, 1.0f };
}

static void fall_task(void *arg)
{
    bool impact_seen = false;
    float impact_g = 0.0f;
    int still_ms = 0;
    TickType_t last_wake = xTaskGetTickCount();

    for (;;) {
        accel_t a = read_accel();
        float mag = sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);

        if (mag > IMPACT_G) {
            impact_seen = true;
            impact_g = mag;
            still_ms = 0;
        } else if (impact_seen) {
            if (fabsf(mag - 1.0f) < STILL_TOLERANCE_G) {
                still_ms += SAMPLE_PERIOD_MS;
                if (still_ms >= STILL_TIME_MS) {
                    ESP_LOGW(TAG, "fall detected, impact %.1f g", impact_g);
                    saathi_publish(SAATHI_EVT_FALL, impact_g);
                    impact_seen = false;
                }
            } else {
                still_ms = 0;   // moving again: probably not a fall
            }
        }

        // TODO (Lead): stand-up detection from the gyroscope + posture change,
        // then saathi_publish(SAATHI_EVT_STAND_UP, 0).

        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(SAMPLE_PERIOD_MS));
    }
}

esp_err_t fall_detect_init(void)
{
    BaseType_t ok = xTaskCreatePinnedToCore(fall_task, "fall_detect", 4096, NULL,
                                            SAATHI_PRIO_FALL, NULL, SAATHI_CORE_SAFETY);
    return ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}
