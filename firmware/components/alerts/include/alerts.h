#pragma once

// SOS over GSM SMS, buzzer, missed-call reply.
// Owner: Member 1 (logic) + Member 2 (GSM wiring).

#include "esp_err.h"

esp_err_t alerts_init(void);
