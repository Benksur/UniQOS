/**
 * @file audio_task.h
 * @brief Audio playback and recording task
 * @ingroup audio_task
 *
 * FreeRTOS task that manages audio codec operations including
 * volume control, output selection, and sound effects playback.
 */

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

/** @ingroup audio_task
 *  @brief Stack size for audio task in bytes */
#define AUDIO_TASK_STACK_SIZE 1024

/** @ingroup audio_task
 *  @brief Audio task priority */
#define AUDIO_TASK_PRIORITY osPriorityHigh

/**
 * @brief Audio command enumeration
 * @ingroup audio_task
 */
typedef enum
{
    AUDIO_VOLUME_UP,            /**< Increase volume */
    AUDIO_VOLUME_DOWN,          /**< Decrease volume */
    AUDIO_MUTE_OUTPUT,          /**< Mute audio output */
    AUDIO_UNMUTE_OUTPUT,        /**< Unmute audio output */
    AUDIO_MUTE_MIC,             /**< Mute microphone */
    AUDIO_UNMUTE_MIC,           /**< Unmute microphone */
    AUDIO_SET_VOLUME_SPK,       /**< Set speaker volume */
    AUDIO_SET_VOLUME_HP,        /**< Set headphone volume */
    AUDIO_SET_VOLUME_MIC,       /**< Set microphone volume */
    AUDIO_SET_VOLUME_HP_MIC,    /**< Set headphone microphone volume */
    AUDIO_SELECT_OUTPUT,        /**< Select audio output */
    AUDIO_SELECT_MIC,           /**< Select microphone input */
    AUDIO_PLAY_TICK,            /**< Play tick sound */
    AUDIO_PLAY_BLOOP,           /**< Play bloop sound */
    AUDIO_GET_CURRENT_VOLUME,   /**< Get current volume level */
    AUDIO_SEND_VOLUME_TO_INPUT, /**< Send volume to input task */
} AudioCommand;

/**
 * @brief Audio output selection
 * @ingroup audio_task
 */
typedef enum
{
    AUDIO_OUTPUT_SPK, /**< Speaker output */
    AUDIO_OUTPUT_HP   /**< Headphone output */
} AudioOutput;

/**
 * @brief Microphone input selection
 * @ingroup audio_task
 */
typedef enum
{
    AUDIO_MIC_INT, /**< Internal microphone */
    AUDIO_MIC_HP   /**< Headphone microphone */
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