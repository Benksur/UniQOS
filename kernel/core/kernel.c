#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"


// Task handles
TaskHandle_t cellular_task_handle;
TaskHandle_t audio_task_handle;
TaskHandle_t display_task_handle;
TaskHandle_t power_task_handle;
TaskHandle_t call_state_task_handle;
TaskHandle_t input_task_handle;
TaskHandle_t watchdog_task_handle;
TaskHandle_t logging_task_handle;
TaskHandle_t sensor_task_handle;

// Queues for inter-task communication
QueueHandle_t cellular_to_call_state_queue;
QueueHandle_t call_state_to_audio_queue;
QueueHandle_t input_to_display_queue;
QueueHandle_t display_to_call_state_queue;

// Event groups
EventGroupHandle_t system_events;

void kernel_init(void)
{
    //placeholders
    // cellular_to_call_state_queue = xQueueCreate(10, sizeof(cellular_event_t));
    // call_state_to_audio_queue = xQueueCreate(5, sizeof(audio_command_t));
    // input_to_display_queue = xQueueCreate(10, sizeof(input_event_t));
    // display_to_call_state_queue = xQueueCreate(5, sizeof(display_event_t));
    
    // system_events = xEventGroupCreate();
    
    // xTaskCreate(cellular_task_main, "Cellular", 1024, NULL, 3, &cellular_task_handle);
    // xTaskCreate(audio_task_main, "Audio", 1024, NULL, 3, &audio_task_handle);
    // xTaskCreate(display_task_main, "Display", 1024, NULL, 2, &display_task_handle);
    // xTaskCreate(power_task_main, "Power", 512, NULL, 2, &power_task_handle);
    // xTaskCreate(call_state_task_main, "CallState", 1024, NULL, 2, &call_state_task_handle);
    // xTaskCreate(input_task_main, "Input", 512, NULL, 1, &input_task_handle);
    // xTaskCreate(watchdog_task_main, "Watchdog", 256, NULL, 2, &watchdog_task_handle);
    // xTaskCreate(logging_task_main, "Logging", 512, NULL, 1, &logging_task_handle);
    // xTaskCreate(sensor_task_main, "Sensor", 512, NULL, 1, &sensor_task_handle);
}