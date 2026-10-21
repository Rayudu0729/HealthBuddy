// Power modes. The power component decides WHEN to switch; modules read
// bindu_get_mode() to decide HOW MUCH work to do.

#include "esp_log.h"
#include "binduos.h"

static const char *TAG = "bindu_mode";
static volatile bindu_mode_t s_mode = BINDU_MODE_DAY;

const char *bindu_mode_name(bindu_mode_t mode)
{
    switch (mode) {
    case BINDU_MODE_DAY:    return "DAY";
    case BINDU_MODE_NIGHT:  return "NIGHT";
    case BINDU_MODE_ALERT:  return "ALERT";
    case BINDU_MODE_SAFETY: return "SAFETY";
    default:                 return "UNKNOWN";
    }
}

void bindu_set_mode(bindu_mode_t mode)
{
    if (mode != s_mode) {
        ESP_LOGI(TAG, "mode %s -> %s", bindu_mode_name(s_mode), bindu_mode_name(mode));
    }
    s_mode = mode;
}

bindu_mode_t bindu_get_mode(void)
{
    return s_mode;
}
