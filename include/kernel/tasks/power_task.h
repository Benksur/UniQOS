#ifndef POWER_TASK_H_
#define POWER_TASK_H_

#include <stdbool.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task_types.h"
#include "queue.h"
#include "cmsis_os2.h"
#include "display_task.h"

#define POWER_TASK_STACK_SIZE 512
#define POWER_TASK_PRIORITY osPriorityLow

typedef enum
{
    POWER_CMD_INIT,
    POWER_CMD_GET_SOC,
    POWER_CMD_GET_VOLTAGE,
    POWER_CMD_GET_TEMPERATURE,
    POWER_CMD_SHUTDOWN,
} PowerCommand;

typedef struct
{
    PowerCommand cmd;
    void *data;
} PowerMessage;

typedef struct PowerTaskContext PowerTaskContext;

PowerTaskContext *PowerTask_Init(DisplayTaskContext *display_ctx);
bool PowerTask_PostCommand(PowerTaskContext *ctx, PowerCommand cmd, void *data);

#endif // POWER_TASK_H_
