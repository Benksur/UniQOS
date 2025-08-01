#include "display_task.h"
#include "display.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "cmsis_os2.h"
#include <string.h>

#define DISPLAY_TASK_STACK_SIZE 512
#define DISPLAY_QUEUE_SIZE 5

static TaskHandle_t display_task_handle = NULL;
static QueueHandle_t display_queue = NULL;

void display_task_main(void *pvParameters)
{
    display_event_t event;
    
    // Initialize display
    display_init();
    display_fill(COLOUR_BLACK);
    
    while (1) {
        if (xQueueReceive(display_queue, &event, pdMS_TO_TICKS(100)) == pdTRUE) {
            
            switch (event.cmd) {
                case DISPLAY_CMD_CLEAR:
                    display_fill(COLOUR_BLACK);
                    break;
                    
                case DISPLAY_CMD_DRAW_TEXT:
                    display_draw_string(event.x, event.y, event.text, COLOUR_WHITE, COLOUR_BLACK, 1);
                    break;
                    
                case DISPLAY_CMD_DRAW_BATTERY:
                    display_draw_battery_icon(event.x, event.y, event.battery_percent, COLOUR_WHITE);
                    break;
                    
                case DISPLAY_CMD_DRAW_SIGNAL:
                    display_draw_signal_bars(event.x, event.y, event.signal_strength, COLOUR_WHITE);
                    break;
                    
                case DISPLAY_CMD_SHOW_CALL:
                    display_fill(COLOUR_GREEN);
                    display_draw_string(50, 100, event.text, COLOUR_BLACK, COLOUR_GREEN, 2);
                    break;
                    
                default:
                    break;
            }
        }
        screen_tick();
    }
}

void display_task_init(void)
{
    display_queue = xQueueCreate(DISPLAY_QUEUE_SIZE, sizeof(display_event_t));
    
    xTaskCreate(display_task_main, "Display", DISPLAY_TASK_STACK_SIZE, NULL, 
                osPriorityNormal, &display_task_handle);
}

task_status_t display_send_command(display_cmd_t cmd, uint16_t x, uint16_t y, const char *text)
{
    if (display_queue == NULL) {
        return TASK_ERROR;
    }
    
    display_event_t event = {0};
    event.cmd = cmd;
    event.x = x;
    event.y = y;
    
    if (text != NULL) {
        strncpy(event.text, text, sizeof(event.text) - 1);
    }
    
    if (xQueueSend(display_queue, &event, pdMS_TO_TICKS(10)) == pdTRUE) {
        return TASK_OK;
    }
    
    return TASK_ERROR;
}