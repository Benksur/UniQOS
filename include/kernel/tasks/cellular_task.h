#ifndef CELLULAR_TASK_H
#define CELLULAR_TASK_H

#include "FreeRTOS.h"
#include "task_types.h"
#include "queue.h"
#include "cmsis_os2.h"
#include "display_task.h"
#include "call_state.h"
#include "modem.h"
#include "stm32_config.h"
#include "sms_types.h"

#define CELLULAR_TASK_STACK_SIZE 2048
#define CELLULAR_TASK_PRIORITY osPriorityNormal

typedef enum
{
    CELLULAR_CMD_INIT,
    CELLULAR_CMD_SLEEP,
    CELLULAR_CMD_AIRPLANE_MODE_ON,
    CELLULAR_CMD_AIRPLANE_MODE_OFF,
    CELLULAR_CMD_SEND_SMS,
    CELLULAR_CMD_DIAL,
    CELLULAR_CMD_HANG_UP,
} CellularCommand;

typedef struct
{
    CellularCommand cmd;
    void *data;
} CellularMessage;

typedef struct CellularTaskContext CellularTaskContext;

CellularTaskContext *CellularTask_Init(DisplayTaskContext *display_ctx, CallStateContext *call_ctx);
bool CellularTask_PostCommand(CellularTaskContext *ctx, CellularCommand cmd, void *data);

#endif