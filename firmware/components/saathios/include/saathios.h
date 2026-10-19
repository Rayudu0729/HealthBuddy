#pragma once

// SaathiOS: health-first runtime on FreeRTOS + ESP-IDF.
// Owner: Lead.

#include <stdint.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

// ---- Cores: safety work never shares a core with the network ----
#define SAATHI_CORE_NET     0   // Wi-Fi, Bluetooth, GSM, sync
#define SAATHI_CORE_SAFETY  1   // fall detection, wake word, voice

// ---- Health-priority scheduler: higher number pre-empts lower ----
#define SAATHI_PRIO_FALL     (configMAX_PRIORITIES - 2)
#define SAATHI_PRIO_BUS      (configMAX_PRIORITIES - 3)
#define SAATHI_PRIO_SOS      (configMAX_PRIORITIES - 4)
#define SAATHI_PRIO_WAKEWORD (configMAX_PRIORITIES - 5)
#define SAATHI_PRIO_VOICE    8
#define SAATHI_PRIO_VITALS   6
#define SAATHI_PRIO_GPS      4
#define SAATHI_PRIO_SYNC     3
#define SAATHI_PRIO_POWER    2

// ---- Event bus ----
typedef enum {
    SAATHI_EVT_FALL,
    SAATHI_EVT_STAND_UP,
    SAATHI_EVT_RISK_HIGH,
    SAATHI_EVT_WAKE_WORD,
    SAATHI_EVT_SOS_BUTTON,
    SAATHI_EVT_MISSED_CALL,
    SAATHI_EVT_LOW_BATTERY,
    SAATHI_EVT_COUNT
} saathi_event_id_t;

typedef struct {
    saathi_event_id_t id;
    int64_t timestamp_us;   // esp_timer time at publish
    float value;            // event-specific: impact g, risk score, battery %, ...
} saathi_event_t;

typedef void (*saathi_event_handler_t)(const saathi_event_t *evt, void *ctx);

esp_err_t saathi_init(void);
esp_err_t saathi_subscribe(saathi_event_id_t id, saathi_event_handler_t handler, void *ctx);
esp_err_t saathi_publish(saathi_event_id_t id, float value);
const char *saathi_event_name(saathi_event_id_t id);

// ---- Power modes ----
typedef enum {
    SAATHI_MODE_DAY,     // all sensors, wake word listening, regular sync
    SAATHI_MODE_NIGHT,   // wake word off, motion interrupt wakes device
    SAATHI_MODE_ALERT,   // everything on: GPS, GSM, voice, buzzer
    SAATHI_MODE_SAFETY,  // low battery or fault: only fall detection + SOS
} saathi_mode_t;

void saathi_set_mode(saathi_mode_t mode);
saathi_mode_t saathi_get_mode(void);
const char *saathi_mode_name(saathi_mode_t mode);

#ifdef __cplusplus
}
#endif
