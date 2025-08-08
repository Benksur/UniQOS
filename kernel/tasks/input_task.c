#include "input_task.h"
#include "input.h"
#include "keypad.h"
#include "display_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os2.h"

#define INPUT_TASK_STACK_SIZE 512
#define INPUT_TASK_PRIORITY osPriorityNormal

static TaskHandle_t input_task_handle = NULL;

static uint32_t prev_button_states[2] = {0};

void input_task_main(void *pvParameters) {
    input_event_t event;
    
    keypad_init();
    
    while (1) {
        keypad_update_states();
        
        for (uint8_t i = 0; i < keypad_get_button_count(); i++) {
            bool current_state = keypad_is_button_pressed(i);
            bool prev_state = (prev_button_states[0] & (1U << i)) != 0;
            
            if (current_state && !prev_state) {
                event = keypad_get_button_event(i);
                if (event != INPUT_NONE) {
                    // Primary: Send to display task for UI handling
                    display_send_command(DISPLAY_HANDLE_INPUT, &event);
                    
                    // Secondary: Send to queue for other tasks:
                    // - Cellular task (call control, DTMF)
                    // - Audio task (volume control, audio routing)
                    // - Power task (power management, sleep/wake)
                    // - Call state task (call state machine)
                    // - Logging task (input logging, analytics)
                    // - Watchdog task (system health monitoring)
                    // - Sensor task (context awareness)
                    
                }
            }
        }
        
        prev_button_states[0] = 0;
        for (uint8_t i = 0; i < keypad_get_button_count(); i++) {
            if (keypad_is_button_pressed(i)) {
                prev_button_states[0] |= (1U << i);
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(5)); 
    }
}

void input_task_init(void) {
    
    xTaskCreate(input_task_main, "Input", INPUT_TASK_STACK_SIZE, NULL,
                INPUT_TASK_PRIORITY, &input_task_handle);
}

