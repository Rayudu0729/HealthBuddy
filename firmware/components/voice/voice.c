// Voice: wake word on device, then speech-to-text (Sarvam AI / Bhashini),
// LLM agent with tools, and a Telugu reply through the speaker.
// The device also speaks first: stand-up guard and the post-fall check-in.

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "binduos.h"
#include "voice.h"

static const char *TAG = "voice";
static QueueHandle_t s_voice_queue;

static void on_event(const bindu_event_t *evt, void *ctx)
{
    xQueueSend(s_voice_queue, evt, 0);
}

// TODO (Lead): play a pre-recorded Telugu clip from the model/storage partition (ESP-ADF + I2S).
static void say(const char *text)
{
    ESP_LOGI(TAG, "say (stub): %s", text);
}

static void voice_task(void *arg)
{
    bindu_event_t evt;

    for (;;) {
        if (xQueueReceive(s_voice_queue, &evt, portMAX_DELAY) != pdTRUE) {
            continue;
        }
        switch (evt.id) {
        case BINDU_EVT_STAND_UP:
            say("Sit for 30 seconds before walking");
            break;
        case BINDU_EVT_FALL:
            say("Are you okay?");
            // TODO: listen for a reply; no reply -> escalate (Level 2).
            break;
        case BINDU_EVT_WAKE_WORD:
            // TODO: record speech, send to speech-to-text + LLM agent, speak the reply.
            say("Yes?");
            break;
        default:
            break;
        }
    }
}

esp_err_t voice_init(void)
{
    s_voice_queue = xQueueCreate(8, sizeof(bindu_event_t));
    if (s_voice_queue == NULL) {
        return ESP_ERR_NO_MEM;
    }

    bindu_subscribe(BINDU_EVT_STAND_UP, on_event, NULL);
    bindu_subscribe(BINDU_EVT_FALL, on_event, NULL);
    bindu_subscribe(BINDU_EVT_WAKE_WORD, on_event, NULL);

    // TODO (Lead): start the ESP-SR wake word task ("Hey Bindu") at BINDU_PRIO_WAKEWORD;
    // it calls bindu_publish(BINDU_EVT_WAKE_WORD, 0) on detection.

    BaseType_t ok = xTaskCreatePinnedToCore(voice_task, "voice", 6144, NULL,
                                            BINDU_PRIO_VOICE, NULL, BINDU_CORE_SAFETY);
    return ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}
