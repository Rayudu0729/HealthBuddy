#pragma once

// Fall alert with 10 s cancel window, SOS alert, buzzer/haptic feedback.
// Sending is a stub; the route will be backend -> httpSMS.
// Owner: Member 1 (logic).

#include "esp_err.h"

esp_err_t alerts_init(void);
