#include "call_state.h"
#include "tim.h"
#include <string.h>

// Call state context structure
struct CallStateContext
{
    CallState current_state;
    EventGroupHandle_t event_group;
    QueueHandle_t queue;
    DisplayTaskContext *display_ctx;
    char caller_id[32];
};

// Helper functions
static void set_vibration(uint8_t enabled, uint8_t strength)
{
    if (enabled)
    {
        drv2603_enable(1);
        drv2603_set_strength_lra(strength);
    }
    else
    {
        drv2603_enable(0);
    }
}

static void set_gpio_call_mode(uint8_t enabled)
{
    HAL_GPIO_WritePin(AUDIO_SW_PORT, AUDIO_SW_PIN, enabled ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void update_display(CallStateContext *ctx)
{
    if (!ctx->display_ctx)
        return;

    // Post display commands with just the caller_id string
    // Display task will handle the UI rendering
    switch (ctx->current_state)
    {
    case CALL_STATE_RINGING:
        // Incoming call - show incoming call overlay
        DisplayTask_PostCommand(ctx->display_ctx, DISPLAY_INCOMING_CALL, ctx->caller_id);
        break;

    case CALL_STATE_ACTIVE:
        // Call picked up - show active call screen
        DisplayTask_PostCommand(ctx->display_ctx, DISPLAY_ACTIVE_CALL, ctx->caller_id);
        break;

    case CALL_STATE_ENDING:
        // Call hung up - return to previous screen or idle
        DisplayTask_PostCommand(ctx->display_ctx, DISPLAY_CALL_ENDED, NULL);
        break;

    case CALL_STATE_DIALLING:
        // Dialling out - show dialling screen
        DisplayTask_PostCommand(ctx->display_ctx, DISPLAY_DIALLING, ctx->caller_id);
        break;

    case CALL_STATE_IDLE:
    default:
        // No call - ensure we're back to normal screen
        // This might not need any special display command
        break;
    }
}

// Process call commands directly
static void process_call_command(CallStateContext *ctx, CallMessage *msg)
{
    switch (msg->cmd)
    {
    case CALL_CMD_INCOMING_CALL:
        // Accept incoming call if idle OR already ringing (allows testing/call replacement)
        if (ctx->current_state == CALL_STATE_IDLE || ctx->current_state == CALL_STATE_RINGING)
        {
            ctx->current_state = CALL_STATE_RINGING;
            if (msg->data)
            {
                CallData *call_data = (CallData *)msg->data;
                strncpy(ctx->caller_id, call_data->caller_id, sizeof(ctx->caller_id) - 1);
                ctx->caller_id[sizeof(ctx->caller_id) - 1] = '\0';
            }
            xEventGroupSetBits(ctx->event_group, CALL_EVENT_INCOMING | CALL_EVENT_DISPLAY);
        }
        break;

    case CALL_CMD_ANSWER_CALL:
        if (ctx->current_state == CALL_STATE_RINGING)
        {
            ctx->current_state = CALL_STATE_ACTIVE;
            xEventGroupSetBits(ctx->event_group, CALL_EVENT_ANSWER | CALL_EVENT_DISPLAY);
        }
        break;

    case CALL_CMD_DIALLING:
        if (ctx->current_state == CALL_STATE_IDLE)
        {
            ctx->current_state = CALL_STATE_DIALLING;
            if (msg->data)
            {
                CallData *call_data = (CallData *)msg->data;
                strncpy(ctx->caller_id, call_data->caller_id, sizeof(ctx->caller_id) - 1);
                ctx->caller_id[sizeof(ctx->caller_id) - 1] = '\0';
            }
            xEventGroupSetBits(ctx->event_group, CALL_EVENT_ANSWER | CALL_EVENT_DISPLAY);
        }
        break;

    case CALL_CMD_HANGUP_CALL:
        if (ctx->current_state != CALL_STATE_IDLE)
        {
            ctx->current_state = CALL_STATE_ENDING;
            xEventGroupSetBits(ctx->event_group, CALL_EVENT_HANGUP | CALL_EVENT_DISPLAY);
            osDelay(100);
            ctx->current_state = CALL_STATE_IDLE;
        }
        break;
    }
}

// Main task function
static void call_state_task_main(void *pvParameters)
{
    CallStateContext *ctx = (CallStateContext *)pvParameters;
    CallMessage msg;
    EventBits_t event_bits;

    // Initialize hardware
    drv2603_init();

    // Main task loop
    for (;;)
    {
        // Wait for events with timeout
        event_bits = xEventGroupWaitBits(
            ctx->event_group,
            CALL_EVENT_INCOMING | CALL_EVENT_ANSWER | CALL_EVENT_HANGUP | CALL_EVENT_DISPLAY,
            pdTRUE,            // Clear bits on exit
            pdFALSE,           // Wait for any bit
            pdMS_TO_TICKS(100) // 100ms timeout
        );

        // Process events
        if (event_bits & CALL_EVENT_INCOMING)
        {
            // Control vibration based on call state
            if (ctx->current_state == CALL_STATE_RINGING)
            {
                set_vibration(1, 100); // Turn ON vibration for incoming call
            }
            else
            {
                set_vibration(0, 0); // Turn OFF vibration when not ringing
            }
        }

        if (event_bits & CALL_EVENT_ANSWER)
        {
            // Call answered or dialling - stop vibration, switch GPIO
            set_vibration(0, 0);
            set_gpio_call_mode(1); // Enable call mode
        }

        if (event_bits & CALL_EVENT_HANGUP)
        {
            // Call ended - stop vibration, reset GPIO
            set_vibration(0, 0);
            set_gpio_call_mode(0); // Disable call mode
        }

        if (event_bits & CALL_EVENT_DISPLAY)
        {
            // Update display
            update_display(ctx);
        }

        // Process any queued commands
        while (xQueueReceive(ctx->queue, &msg, 0))
        {
            process_call_command(ctx, &msg);
        }

        // Yield to other tasks - critical for system responsiveness
        osDelay(50);
    }
}

// Public API implementation
CallStateContext *CallState_Init(DisplayTaskContext *display_ctx)
{
    static CallStateContext call_ctx;
    memset(&call_ctx, 0, sizeof(call_ctx));

    // Initialize context
    call_ctx.current_state = CALL_STATE_IDLE;
    call_ctx.display_ctx = display_ctx;

    // Create event group
    call_ctx.event_group = xEventGroupCreate();
    if (!call_ctx.event_group)
    {
        return NULL;
    }

    // Create queue
    call_ctx.queue = xQueueCreate(5, sizeof(CallMessage));
    if (!call_ctx.queue)
    {
        vEventGroupDelete(call_ctx.event_group);
        return NULL;
    }

    // Create and start the task
    osThreadAttr_t task_attr = {
        .name = "CallStateTask",
        .stack_size = CALL_STATE_TASK_STACK_SIZE,
        .priority = CALL_STATE_TASK_PRIORITY};

    osThreadId_t thread_id = osThreadNew(call_state_task_main, &call_ctx, &task_attr);
    if (!thread_id)
    {
        vQueueDelete(call_ctx.queue);
        vEventGroupDelete(call_ctx.event_group);
        return NULL;
    }

    return &call_ctx;
}

void CallState_SetDisplayContext(CallStateContext *ctx, DisplayTaskContext *display_ctx)
{
    if (ctx)
    {
        ctx->display_ctx = display_ctx;
    }
}

bool CallState_PostCommand(CallStateContext *ctx, CallCommand cmd, void *data)
{
    if (!ctx || !ctx->queue)
    {
        return false;
    }

    CallMessage msg = {
        .cmd = cmd,
        .data = data};

    return xQueueSend(ctx->queue, &msg, pdMS_TO_TICKS(10)) == pdTRUE;
}

CallState CallState_GetCurrentState(CallStateContext *ctx)
{
    if (!ctx)
    {
        return CALL_STATE_IDLE;
    }

    return ctx->current_state;
}
