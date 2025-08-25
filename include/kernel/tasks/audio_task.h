#ifndef AUDIO_TASK_H_
#define AUDIO_TASK_H_

#include <stdbool.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task_types.h"
#include "queue.h"
#include "cmsis_os2.h"
#include "nau88c22.h"

typedef enum {
    AUDIO_OUTPUT_SPK,
    AUDIO_OUTPUT_HP
} AudioOutput;

typedef enum {
    AUDIO_MIC_INT,
    AUDIO_MIC_HP
} AudioMic;

typedef enum {
    AUDIO_VOLUME_UP,
    AUDIO_VOLUME_DOWN,
    AUDIO_MUTE_OUTPUT,
    AUDIO_UNMUTE_OUTPUT,
    AUDIO_MUTE_MIC,
    AUDIO_UNMUTE_MIC,
    AUDIO_SET_VOLUME_SPK,
    AUDIO_SET_VOLUME_HP,
    AUDIO_SET_VOLUME_MIC,
    AUDIO_SET_VOLUME_HP_MIC,
    AUDIO_SELECT_OUTPUT,
    AUDIO_SELECT_MIC,
} AudioCommand;

typedef struct {
    AudioCommand cmd;
    void* data;
} AudioMessage;

typedef struct {
    uint8_t volume_speaker;
    uint8_t volume_headphones;
    uint8_t volume_mic_int;
    uint8_t volume_mic_ext;
    bool output_muted;
    bool mic_muted;
    AudioOutput selected_output;
    AudioMic selected_mic;
} AudioUserSettings;

typedef struct {
    AudioUserSettings settings;
    // msg queue and codec driver instances
    QueueHandle_t queue;
    IAudioDriver_t* codec;
} AudioTaskContext;

void audio_task_main(void *pvParameters);
void audio_task_init(void);

#endif