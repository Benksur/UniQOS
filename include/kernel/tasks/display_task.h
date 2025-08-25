#ifndef DISPLAY_TASK_H_
#define DISPLAY_TASK_H_

#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task_types.h"
#include "queue.h"
#include "cmsis_os2.h"
#include "screen.h"
#include "theme.h"
#include "stm32h7xx_hal_rtc.h"

typedef enum {
    DISPLAY_HANDLE_INPUT,
    DISPLAY_SET_PAGE,
    DISPLAY_CLEAR_SCREEN,
    DISPLAY_SET_BATTERY_STATUS,
    DISPLAY_SET_TIME,
    DISPLAY_SET_DATE,
    DISPLAY_CMD_COUNT
} DisplayCommand;

typedef struct {
    DisplayCommand cmd;
    void* data;
} DisplayMessage;

typedef struct {
    uint8_t battery_percent;
    uint8_t signal_strength;
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
} DisplayStatus;


// opaque context
typedef struct DisplayTaskContext DisplayTaskContext;

DisplayTaskContext* DisplayTask_Init(void);
bool DisplayTask_PostCommand(DisplayTaskContext* ctx, DisplayCommand cmd, void* data);

#endif // DISPLAY_TASK_H_