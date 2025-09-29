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

#define INPUT_TASK_STACK_SIZE 512
#define INPUT_TASK_PRIORITY osPriorityNormal

void InputTask_Init(DisplayTaskContext *display_ctx, AudioTaskContext *audio_ctx);

#endif // INPUT_TASK_H_