// Power modes. The power component decides WHEN to switch; modules read
// saathi_get_mode() to decide HOW MUCH work to do.

#include "esp_log.h"
#include "saathios.h"

static const char *TAG = "saathi_mode";
static volatile saathi_mode_t s_mode = SAATHI_MODE_DAY;

const char *saathi_mode_name(saathi_mode_t mode)
{
    switch (mode) {
    case SAATHI_MODE_DAY:    return "DAY";
    case SAATHI_MODE_NIGHT:  return "NIGHT";
    case SAATHI_MODE_ALERT:  return "ALERT";
    case SAATHI_MODE_SAFETY: return "SAFETY";
    default:                 return "UNKNOWN";
    }
}

void saathi_set_mode(saathi_mode_t mode)
{
    if (mode != s_mode) {
        ESP_LOGI(TAG, "mode %s -> %s", saathi_mode_name(s_mode), saathi_mode_name(mode));
    }
    s_mode = mode;
}

saathi_mode_t saathi_get_mode(void)
{
    return s_mode;
}
