#ifndef DISPLAY_TASK_H_
#define DISPLAY_TASK_H_

#include "task_types.h"
#include "stm32h7xx_hal_rtc.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    DISPLAY_HANDLE_INPUT,
} display_cmd_t;

typedef struct {
    display_cmd_t cmd;
    void* data;
    uint8_t battery_percent;
    uint8_t signal_strength;
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
} display_event_t;

void display_task_main(void *pvParameters);
void display_task_init(void);
task_status_t display_send_command(display_cmd_t cmd, void* data);

#endif // DISPLAY_TASK_H_