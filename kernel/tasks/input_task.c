#include "input_task.h"
#include "input.h"
#include "keypad.h"
#include "display_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os2.h"

#define INPUT_TASK_STACK_SIZE 512
#define INPUT_TASK_PRIORITY osPriorityNormal

static uint8_t current_volume = 50;
static DisplayTaskContext *display_ctx = NULL;

void input_task_main(void *pvParameters)
{
    input_event_t event;

    // Initialize keypad
    keypad_init();

    while (1)
    {

            keypad_update_states();
            
            // Check all buttons and handle input events
            for (int button_idx = 0; button_idx < keypad_get_button_count(); button_idx++)
            {
                if (keypad_is_button_pressed(button_idx))
                {
                    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
                    event = keypad_get_button_event(button_idx);

                    // Handle audio-specific events directly
                    if (event == INPUT_VOLUME_UP)
                    {
                        if (current_volume < 100)
                        {
                            current_volume += 5;
                        }

                        // Send volume update to display task (queue is thread-safe)
                        if (display_ctx)
                        {
                            DisplayTask_PostCommand(display_ctx, DISPLAY_SET_VOLUME, &current_volume);
                        }
                    }
                    else if (event == INPUT_VOLUME_DOWN)
                    {
                        if (current_volume > 0)
                        {
                            current_volume -= 5;
                        }

                        // Send volume update to display task (queue is thread-safe)
                        if (display_ctx)
                        {
                            DisplayTask_PostCommand(display_ctx, DISPLAY_SET_VOLUME, &current_volume);
                        }
                    }
                    else
                    {
                        // Dispatch all other input events to display task
                        if (display_ctx)
                        {
                            DisplayTask_PostCommand(display_ctx, DISPLAY_HANDLE_INPUT, &event);
                        }
                    }

                    // Audio feedback removed as requested
                }
            }
        
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void input_task_init(void)
{
    osThreadAttr_t task_attr = {
        .name = "InputTask",
        .stack_size = INPUT_TASK_STACK_SIZE,
        .priority = INPUT_TASK_PRIORITY};
    osThreadNew(input_task_main, NULL, &task_attr);
}

void input_task_set_display_context(DisplayTaskContext *ctx)
{
    // Critical section for shared variable assignment
    taskENTER_CRITICAL();
    display_ctx = ctx;
    taskEXIT_CRITICAL();
}
