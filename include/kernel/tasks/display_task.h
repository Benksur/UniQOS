#ifndef DISPLAY_TASK_H_
#define DISPLAY_TASK_H_

#include "task_types.h"
#include "stm32h7xx_hal_rtc.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    DISPLAY_CMD_CLEAR,
    DISPLAY_CMD_DRAW_TEXT,
    DISPLAY_CMD_DRAW_BATTERY,
    DISPLAY_CMD_DRAW_SIGNAL,
    DISPLAY_CMD_SHOW_CALL
} display_cmd_t;

typedef struct {
    display_cmd_t cmd;
    uint16_t x;
    uint16_t y;
    char text[64];
    uint8_t battery_percent;
    uint8_t signal_strength;
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
} display_event_t;

void display_task_main(void *pvParameters);
void display_task_init(void);
task_status_t display_send_command(display_cmd_t cmd, uint16_t x, uint16_t y, const char *text);

#endif // DISPLAY_TASK_H_