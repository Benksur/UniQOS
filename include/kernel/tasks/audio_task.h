#ifndef AUDIO_TASK_H_
#define AUDIO_TASK_H_

#include "task_types.h"
#include "queue.h"
#include "cmsis_os2.h"
#include "nau88c22.h"

typedef enum {
    AUDIO_VOLUME_UP,
    AUDIO_VOLUME_DOWN,
    AUDIO_MUTE_OUTPUT,
    AUDIO_UNMUTE_OUTPUT,
    AUDIO_MUTE_MIC,
    AUDIO_UNMUTE_MIC,
    AUDIO_MUTE_HP_MIC,
    AUDIO_UNMUTE_HP_MIC,
    AUDIO_SET_EQ,
    AUDIO_SET_LINE_OUT,
    AUDIO_SET_LINE_IN,
} audio_cmd_t;

typedef struct {
    audio_cmd_t cmd;
    void* data;
} audio_event_t;

void audio_task_main(void *pvParameters);
void audio_task_init(void);

#endif