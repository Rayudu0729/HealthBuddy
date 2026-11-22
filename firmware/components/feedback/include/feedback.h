#pragma once

// User feedback: active buzzer (D0 = GPIO1) + DRV2605L haptic motor driver.
// Patterns play from a small queue-fed task, so callers never block.

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BUZZER_GPIO
#define BUZZER_GPIO          1   // D0
#endif
#ifndef BUZZER_ACTIVE_LEVEL
#define BUZZER_ACTIVE_LEVEL  1   // 1 = buzzer sounds when pin is high
#endif

typedef enum {
    FEEDBACK_BOOT_READY,
    FEEDBACK_SOS_CONFIRMED,
    FEEDBACK_FALL_COUNTDOWN_TICK,
    FEEDBACK_ALERT_CANCELLED,
    FEEDBACK_ALERT_SENT,
    FEEDBACK_PATTERN_COUNT
} feedback_pattern_t;

esp_err_t feedback_init(void);
void feedback_play(feedback_pattern_t pattern);   // non-blocking

#ifdef __cplusplus
}
#endif
