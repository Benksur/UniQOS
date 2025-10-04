#include "input_task.h"
#include "call_state.h"
#include "cellular_task.h"

typedef struct
{
    DisplayTaskContext *display_ctx;
    AudioTaskContext *audio_ctx;
    CallStateContext *call_ctx;
} InputTaskContext;

static uint8_t current_volume = 100; // Initialize to match audio task default (speaker volume)

void input_task_main(void *pvParameters)
{
    InputTaskContext *input_ctx = (InputTaskContext *)pvParameters;
    DisplayTaskContext *display_ctx = input_ctx->display_ctx;
    AudioTaskContext *audio_ctx = input_ctx->audio_ctx;
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
                event = keypad_get_button_event(button_idx);

                // Handle test trigger - STAR key triggers incoming call test
                if (event == INPUT_KEYPAD_STAR && input_ctx->call_ctx)
                {
                    static CallData test_call = {
                        .caller_id = "+61413279693"};
                    CallState_PostCommand(input_ctx->call_ctx, CALL_CMD_INCOMING_CALL, &test_call);
                }
                // Handle test trigger - HASH key triggers incoming SMS test
                // else if (event == INPUT_KEYPAD_HASH && display_ctx)
                // {
                //     static ReceivedSms test_sms = {
                //         .sender = "+61412345678",
                //         .body = "Hello! This is a test SMS message from the input task. Testing the SMS notification system."};
                //     DisplayTask_PostCommand(display_ctx, DISPLAY_SHOW_SMS, &test_sms);
                // }
                // Handle audio-specific events
                else if (event == INPUT_VOLUME_UP)
                {
                    // Send volume up command to audio task
                    if (audio_ctx)
                    {
                        AudioTask_PostCommand(audio_ctx, AUDIO_VOLUME_UP, NULL);
                    }

                    // Update local volume for display (keep in sync)
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
                    // Send volume down command to audio task
                    if (audio_ctx)
                    {
                        AudioTask_PostCommand(audio_ctx, AUDIO_VOLUME_DOWN, NULL);
                    }

                    // Update local volume for display (keep in sync)
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
                if (event >= INPUT_KEYPAD_0 && event <= INPUT_KEYPAD_9)
                {
                    if (audio_ctx)
                    {
                        AudioTask_PostCommand(audio_ctx, AUDIO_PLAY_TICK, NULL);
                    }
                }
                else
                {
                    if (audio_ctx)
                    {
                        AudioTask_PostCommand(audio_ctx, AUDIO_PLAY_BLOOP, NULL);
                    }
                }

                // Audio feedback removed as requested
            }
        }

        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void InputTask_Init(DisplayTaskContext *display_ctx, AudioTaskContext *audio_ctx, CallStateContext *call_ctx)
{
    static InputTaskContext input_ctx;
    input_ctx.display_ctx = display_ctx;
    input_ctx.audio_ctx = audio_ctx;
    input_ctx.call_ctx = call_ctx;

    osThreadAttr_t task_attr = {
        .name = "InputTask",
        .stack_size = INPUT_TASK_STACK_SIZE,
        .priority = INPUT_TASK_PRIORITY};
    osThreadNew(input_task_main, &input_ctx, &task_attr);
}
