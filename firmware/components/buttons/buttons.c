// Polled buttons: 10 ms poll, 3 stable samples (30 ms) to debounce.

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "binduos.h"
#include "buttons.h"

static const char *TAG = "buttons";

#define POLL_MS          10
#define DEBOUNCE_SAMPLES 3

typedef struct {
    int gpio;
    bool pressed;       // debounced state
    bool raw_last;
    int stable;         // consecutive samples equal to raw_last
} button_t;

// Returns true when the debounced state changed this call.
static bool button_update(button_t *b)
{
    bool raw = (gpio_get_level(b->gpio) == 0);   // active low
    if (raw == b->raw_last) {
        if (b->stable < DEBOUNCE_SAMPLES) {
            b->stable++;
        }
    } else {
        b->raw_last = raw;
        b->stable = 1;
    }
    if (b->stable >= DEBOUNCE_SAMPLES && b->pressed != raw) {
        b->pressed = raw;
        return true;
    }
    return false;
}

static void buttons_task(void *arg)
{
    button_t sos = { .gpio = BUTTON_SOS_GPIO };
    button_t cancel = { .gpio = BUTTON_CANCEL_GPIO };
    int sos_held_ms = 0;
    bool sos_sent = false;
    TickType_t last_wake = xTaskGetTickCount();

    for (;;) {
        if (button_update(&sos)) {
            if (sos.pressed) {
                ESP_LOGI(TAG, "SOS pressed - hold %d ms", SOS_HOLD_MS);
                sos_held_ms = 0;
                sos_sent = false;
            } else if (!sos_sent) {
                ESP_LOGI(TAG, "SOS released early - ignored");
            }
        }
        if (sos.pressed && !sos_sent) {
            sos_held_ms += POLL_MS;
            if (sos_held_ms >= SOS_HOLD_MS) {
                sos_sent = true;
                ESP_LOGW(TAG, "SOS held %d ms - triggering", SOS_HOLD_MS);
                bindu_publish(BINDU_EVT_SOS_BUTTON, 0);
            }
        }

        if (button_update(&cancel) && cancel.pressed) {
            ESP_LOGI(TAG, "CANCEL pressed");
            bindu_publish(BINDU_EVT_CANCEL, 0);
        }

        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(POLL_MS));
    }
}

esp_err_t buttons_init(void)
{
    gpio_config_t io = {
        .pin_bit_mask = (1ULL << BUTTON_SOS_GPIO) | (1ULL << BUTTON_CANCEL_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    esp_err_t err = gpio_config(&io);
    if (err != ESP_OK) {
        return err;
    }
    BaseType_t ok = xTaskCreatePinnedToCore(buttons_task, "buttons", 3072, NULL,
                                            BINDU_PRIO_SOS, NULL, BINDU_CORE_SAFETY);
    return ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}
