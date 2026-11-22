#pragma once

#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"

// DRV2605L at 0x5A driving an ERM motor, internal trigger, ERM library 1.
esp_err_t drv2605_init(void);
// Play up to 8 library effects in sequence (non-blocking, hardware plays them).
esp_err_t drv2605_play(const uint8_t *effects, size_t n);
