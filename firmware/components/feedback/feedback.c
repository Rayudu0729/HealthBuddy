// Buzzer + haptic patterns. feedback_play() only queues; the feedback task plays.

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "binduos.h"
#include "feedback.h"
#include "drv2605.h"

static const char *TAG = "feedback";

#define MAX_STEPS   12
#define MAX_EFFECTS 4

// Buzzer steps alternate on/off durations in ms, 0-terminated.
typedef struct {
    const char *name;
    uint16_t steps_ms[MAX_STEPS];
    uint8_t effects[MAX_EFFECTS];   // DRV2605 ERM library 1 effect ids, 0-terminated
} pattern_t;

static const pattern_t PATTERNS[FEEDBACK_PATTERN_COUNT] = {
    [FEEDBACK_BOOT_READY]          = { "BOOT_READY",          { 80, 80, 80 },                          { 1 } },
    [FEEDBACK_SOS_CONFIRMED]       = { "SOS_CONFIRMED",       { 300, 150, 300, 150, 300 },             { 14 } },
    [FEEDBACK_FALL_COUNTDOWN_TICK] = { "FALL_COUNTDOWN_TICK", { 80 },                                  { 1 } },
    [FEEDBACK_ALERT_CANCELLED]     = { "ALERT_CANCELLED",     { 600 },                                 { 10 } },
    [FEEDBACK_ALERT_SENT]          = { "ALERT_SENT",          { 60, 60, 60, 60, 60, 60, 60, 60, 60 },  { 52 } },
};

static QueueHandle_t s_queue;

static void buzzer_set(bool on)
{
    gpio_set_level(BUZZER_GPIO, on ? BUZZER_ACTIVE_LEVEL : !BUZZER_ACTIVE_LEVEL);
}

static void play_pattern(const pattern_t *p)
{
    size_t n = 0;
    while (n < MAX_EFFECTS && p->effects[n] != 0) {
        n++;
    }
    drv2605_play(p->effects, n);   // fails harmlessly if the driver is absent

    bool on = true;
    for (int i = 0; i < MAX_STEPS && p->steps_ms[i] != 0; i++) {
        buzzer_set(on);
        vTaskDelay(pdMS_TO_TICKS(p->steps_ms[i]));
        on = !on;
    }
    buzzer_set(false);
}

static void feedback_task(void *arg)
{
    feedback_pattern_t id;
    for (;;) {
        if (xQueueReceive(s_queue, &id, portMAX_DELAY) == pdTRUE && id < FEEDBACK_PATTERN_COUNT) {
            ESP_LOGD(TAG, "pattern %s", PATTERNS[id].name);
            play_pattern(&PATTERNS[id]);
        }
    }
}

esp_err_t feedback_init(void)
{
    gpio_config_t io = {
        .pin_bit_mask = 1ULL << BUZZER_GPIO,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&io));
    buzzer_set(false);

    esp_err_t err = drv2605_init();
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "DRV2605L not found at 0x5A (%s) - haptics off, buzzer only", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "DRV2605L ready (ERM, library 1)");
    }

    s_queue = xQueueCreate(8, sizeof(feedback_pattern_t));
    if (s_queue == NULL) {
        return ESP_ERR_NO_MEM;
    }
    BaseType_t ok = xTaskCreatePinnedToCore(feedback_task, "feedback", 3072, NULL,
                                            BINDU_PRIO_VOICE, NULL, BINDU_CORE_NET);
    return ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}

void feedback_play(feedback_pattern_t pattern)
{
    if (s_queue == NULL || pattern >= FEEDBACK_PATTERN_COUNT) {
        return;
    }
    if (xQueueSend(s_queue, &pattern, 0) != pdTRUE) {
        ESP_LOGW(TAG, "queue full - dropped pattern %s", PATTERNS[pattern].name);
    }
}
