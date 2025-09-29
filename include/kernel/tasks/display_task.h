#ifndef DISPLAY_TASK_H_
#define DISPLAY_TASK_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task_types.h"
#include "queue.h"
#include "cmsis_os2.h"
#include "screen.h"
#include "rtc.h"
#include "display.h"
#include "tile.h"
#include "input.h"
#include "pages/menu.h"
#include "status_bar.h"
#include "task.h"
#include "incoming_call.h"

#define DISPLAY_TASK_STACK_SIZE 2048
#define DISPLAY_TASK_PRIORITY osPriorityNormal

typedef enum
{
    DISPLAY_HANDLE_INPUT,
    DISPLAY_SET_PAGE,
    DISPLAY_CLEAR_SCREEN,
    DISPLAY_SET_BATTERY_STATUS,
    DISPLAY_SET_SIGNAL_STATUS,
    DISPLAY_SET_VOLUME,
    DISPLAY_INCOMING_CALL,
    DISPLAY_CMD_COUNT
} DisplayCommand;

typedef struct
{
    DisplayCommand cmd;
    void *data;
} DisplayMessage;

typedef struct
{
    uint8_t battery_percent;
    uint8_t signal_strength;
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
} DisplayStatus;

// opaque context
typedef struct DisplayTaskContext DisplayTaskContext;

DisplayTaskContext *DisplayTask_Init(void);
bool DisplayTask_PostCommand(DisplayTaskContext *ctx, DisplayCommand cmd, void *data);

#endif // DISPLAY_TASK_H_