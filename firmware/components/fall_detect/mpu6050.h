#pragma once

#include "esp_err.h"

// MPU-6050 at 0x68, accelerometer +/-8 g, ~44 Hz DLPF, 100 Hz sample rate.
esp_err_t mpu6050_init(void);
esp_err_t mpu6050_read_accel_g(float *x, float *y, float *z);
