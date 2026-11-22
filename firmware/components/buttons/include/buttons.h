#pragma once

// SOS (hold 1.5 s) and CANCEL (short press) buttons, wired to GND, internal pull-ups.

#include "esp_err.h"

#define BUTTON_SOS_GPIO     2     // D1
#define BUTTON_CANCEL_GPIO  3     // D2
#define SOS_HOLD_MS         1500

esp_err_t buttons_init(void);
