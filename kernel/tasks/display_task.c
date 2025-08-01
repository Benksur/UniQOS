#include "display_task.h"
#include "display.h"
#include "input.h"
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

    display_init();
    display_fill(COLOUR_BLACK);
    
    while (1) {
        if (xQueueReceive(display_queue, &event, pdMS_TO_TICKS(100)) == pdTRUE) {
            
            switch (event.cmd) {
                case DISPLAY_HANDLE_INPUT:
                    screen_handle_input((input_event_t*)event.data);
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

task_status_t display_send_command(display_cmd_t cmd, void* data)
{
    if (display_queue == NULL) {
        return TASK_ERROR;
    }
    
    display_event_t event = {0};
    event.cmd = cmd;
    event.data = data;
    
    if (xQueueSend(display_queue, &event, pdMS_TO_TICKS(10)) == pdTRUE) {
        return TASK_OK;
    }
    
    return TASK_ERROR;
}