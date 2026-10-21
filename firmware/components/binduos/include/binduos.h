#pragma once

// BinduOS: health-first runtime on FreeRTOS + ESP-IDF.
// Owner: Lead.

#include <stdint.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

// ---- Cores: safety work never shares a core with the network ----
#define BINDU_CORE_NET     0   // Wi-Fi, Bluetooth, GSM, sync
#define BINDU_CORE_SAFETY  1   // fall detection, wake word, voice

// ---- Health-priority scheduler: higher number pre-empts lower ----
#define BINDU_PRIO_FALL     (configMAX_PRIORITIES - 2)
#define BINDU_PRIO_BUS      (configMAX_PRIORITIES - 3)
#define BINDU_PRIO_SOS      (configMAX_PRIORITIES - 4)
#define BINDU_PRIO_WAKEWORD (configMAX_PRIORITIES - 5)
#define BINDU_PRIO_VOICE    8
#define BINDU_PRIO_VITALS   6
#define BINDU_PRIO_GPS      4
#define BINDU_PRIO_SYNC     3
#define BINDU_PRIO_POWER    2

// ---- Event bus ----
typedef enum {
    BINDU_EVT_FALL,
    BINDU_EVT_STAND_UP,
    BINDU_EVT_RISK_HIGH,
    BINDU_EVT_WAKE_WORD,
    BINDU_EVT_SOS_BUTTON,
    BINDU_EVT_MISSED_CALL,
    BINDU_EVT_LOW_BATTERY,
    BINDU_EVT_COUNT
} bindu_event_id_t;

typedef struct {
    bindu_event_id_t id;
    int64_t timestamp_us;   // esp_timer time at publish
    float value;            // event-specific: impact g, risk score, battery %, ...
} bindu_event_t;

typedef void (*bindu_event_handler_t)(const bindu_event_t *evt, void *ctx);

esp_err_t bindu_init(void);
esp_err_t bindu_subscribe(bindu_event_id_t id, bindu_event_handler_t handler, void *ctx);
esp_err_t bindu_publish(bindu_event_id_t id, float value);
const char *bindu_event_name(bindu_event_id_t id);

// ---- Power modes ----
typedef enum {
    BINDU_MODE_DAY,     // all sensors, wake word listening, regular sync
    BINDU_MODE_NIGHT,   // wake word off, motion interrupt wakes device
    BINDU_MODE_ALERT,   // everything on: GPS, GSM, voice, buzzer
    BINDU_MODE_SAFETY,  // low battery or fault: only fall detection + SOS
} bindu_mode_t;

void bindu_set_mode(bindu_mode_t mode);
bindu_mode_t bindu_get_mode(void);
const char *bindu_mode_name(bindu_mode_t mode);

#ifdef __cplusplus
}
#endif
