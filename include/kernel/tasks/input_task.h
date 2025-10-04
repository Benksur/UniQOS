/**
 * @file input_task.h
 * @brief Input handling task interface
 * @ingroup input_task
 *
 * Provides the input task interface for processing user input from the keypad
 * and coordinating with display and audio tasks. Handles input events and
 * distributes them to appropriate system components.
 */

#ifndef INPUT_TASK_H_
#define INPUT_TASK_H_

#include <stdbool.h>
#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "task.h"
#include "task_types.h"
#include "input.h"
#include "keypad.h"
#include "display_task.h"
#include "audio_task.h"

/**
 * @brief Forward declaration of call state context
 * @ingroup input_task
 *
 * Forward declaration to avoid circular dependencies.
 * Defined in call_state.h.
 */
typedef struct CallStateContext CallStateContext;

/** @ingroup input_task
 *  @brief Stack size for input task in words */
#define INPUT_TASK_STACK_SIZE 512

/** @ingroup input_task
 *  @brief Priority level for input task */
#define INPUT_TASK_PRIORITY osPriorityNormal

/**
 * @ingroup input_task
 * @brief Initialize the input handling task
 * @param display_ctx Pointer to display task context
 * @param audio_ctx Pointer to audio task context
 * @param call_ctx Pointer to call state context
 *
 * Initializes the input task with references to other system tasks.
 * The input task processes keypad events and coordinates with display,
 * audio, and call state management tasks.
 */
void InputTask_Init(DisplayTaskContext *display_ctx, AudioTaskContext *audio_ctx, CallStateContext *call_ctx);

#endif // INPUT_TASK_H_