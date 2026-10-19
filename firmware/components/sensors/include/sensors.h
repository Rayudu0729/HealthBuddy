#pragma once

// Vitals (MAX30102, MLX90614) and GPS (NEO-6M) readings.
// Owner: Member 2.

#include "esp_err.h"

esp_err_t sensors_init(void);
