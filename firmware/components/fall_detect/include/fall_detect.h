#pragma once

// Fall and stand-up detection from the MPU6050 at 100 Hz.
// Owner: Member 2.

#include "esp_err.h"

esp_err_t fall_detect_init(void);
