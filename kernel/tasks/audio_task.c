#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "cmsis_os2.h"
#include "nau88c22.h"
#include "audio_task.h"

#define AUDIO_TASK_STACK_SIZE 512
#define AUDIO_TASK_PRIORITY osPriorityNormal
#define AUDIO_QUEUE_SIZE 5

static TaskHandle_t audio_task_handle = NULL;
static QueueHandle_t audio_queue = NULL;
static audio_event_t event;



void audio_task_main(void *pvParameters) {
    uint8_t status;
    nau88c22_codec_t codec;

    status = nau88c22_init(&codec);
    while(1) {
        if (xQueueReceive(audio_queue, &event, pdMS_TO_TICKS(10)) == pdTRUE) {
            switch (event.cmd) {
                case AUDIO_VOLUME_UP:
                    nau88c22_increment_output_volume(&codec, 1);
                    break;
                case AUDIO_VOLUME_DOWN:
                    nau88c22_increment_output_volume(&codec, -1);
                    break;
                case AUDIO_MUTE_OUTPUT:
                    nau88c22_mute_output(&codec, 1);
                    break;
            }
        }
    }


}

void audio_task_init(void) {
    audio_queue = xQueueCreate(AUDIO_QUEUE_SIZE, sizeof(audio_event_t));

    xTaskCreate(audio_task_main, "Audio", AUDIO_TASK_STACK_SIZE, NULL,
                osPriorityNormal, &audio_task_handle);
}