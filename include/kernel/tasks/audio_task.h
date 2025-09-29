#ifndef AUDIO_TASK_H_
#define AUDIO_TASK_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task_types.h"
#include "queue.h"
#include "cmsis_os2.h"
#include "nau88c22.h"
#include "i2s.h"
#include "tick_sound.h"
#include "bloop_optimized.h"

#define AUDIO_TASK_STACK_SIZE 1024
#define AUDIO_TASK_PRIORITY osPriorityLow

typedef enum
{
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
    AUDIO_PLAY_TICK,
    AUDIO_PLAY_BLOOP,
    AUDIO_GET_CURRENT_VOLUME,
    AUDIO_SEND_VOLUME_TO_INPUT,
} AudioCommand;

typedef enum
{
    AUDIO_OUTPUT_SPK,
    AUDIO_OUTPUT_HP
} AudioOutput;

typedef enum
{
    AUDIO_MIC_INT,
    AUDIO_MIC_HP
} AudioMic;

typedef struct
{
    AudioCommand cmd;
    void *data;
} AudioMessage;

typedef struct AudioTaskContext AudioTaskContext;

// public API
AudioTaskContext *AudioTask_Init(void);
bool AudioTask_PostCommand(AudioTaskContext *ctx, AudioCommand cmd, void *data);

#endif