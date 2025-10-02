#include "display_task.h"
#include "call_state.h"

struct DisplayTaskContext
{
    QueueHandle_t queue;
    CallStateContext *call_ctx; // Reference to call state for callbacks
};

typedef void (*DisplayCmdHandler)(DisplayTaskContext *ctx, DisplayMessage *msg);

/* ===== CALLBACK FOR INCOMING CALL OVERLAY ===== */
static void incoming_call_callback(int action, void *user_data)
{
    CallStateContext *call_ctx = (CallStateContext *)user_data;
    if (!call_ctx)
        return;

    switch (action)
    {
    case INCOMING_CALL_ACTION_PICKUP:
        // Answer the call
        CallState_PostCommand(call_ctx, CALL_CMD_ANSWER_CALL, NULL);
        break;
    case INCOMING_CALL_ACTION_HANGUP:
        // Reject/hangup the call
        CallState_PostCommand(call_ctx, CALL_CMD_HANGUP_CALL, NULL);
        break;
    default:
        break;
    }
}

/* ===== HANDLERS ===== */
static void handle_input_event(DisplayTaskContext *ctx, DisplayMessage *msg)
{
    input_event_t *event = (input_event_t *)msg->data;
    if (event)
    {
        // Handle special cases like the test file
        if (*event == INPUT_RIGHT)
        {
            screen_pop_page();
        }
        else
        {
            screen_handle_input(*event);
        }
    }
}

static void handle_set_page(DisplayTaskContext *ctx, DisplayMessage *msg)
{
    Page *page = (Page *)msg->data;
    if (page)
    {
        screen_push_page(page);
    }
}

static void handle_clear_screen(DisplayTaskContext *ctx, DisplayMessage *msg)
{
    display_fill(COLOUR_BLACK);
}

static void handle_set_signal_status(DisplayTaskContext *ctx, DisplayMessage *msg)
{
    uint8_t *signal = (uint8_t *)msg->data;
    if (signal)
    {
        status_bar_update_signal(*signal);
    }
}

static void handle_set_battery_status(DisplayTaskContext *ctx, DisplayMessage *msg)
{
    uint8_t *battery = (uint8_t *)msg->data;
    if (battery)
    {
        status_bar_update_battery(*battery);
    }
}

static void handle_set_volume(DisplayTaskContext *ctx, DisplayMessage *msg)
{
    uint8_t *volume = (uint8_t *)msg->data;
    if (volume)
    {
        status_bar_show_volume(*volume);
    }
}

static void handle_incoming_call(DisplayTaskContext *ctx, DisplayMessage *msg)
{
    char *caller_id = (char *)msg->data;
    if (caller_id && ctx->call_ctx)
    {
        Page *incoming_call_page = incoming_call_overlay_create(caller_id, incoming_call_callback, ctx->call_ctx);
        screen_push_page(incoming_call_page);
    }
}

static void handle_active_call(DisplayTaskContext *ctx, DisplayMessage *msg)
{
    char *caller_id = (char *)msg->data;
    if (caller_id)
    {
        // TODO: Create active call page/overlay
        // Page *active_call_page = active_call_overlay_create(caller_id, NULL, NULL);
        // screen_push_page(active_call_page);
    }
}

static void handle_call_ended(DisplayTaskContext *ctx, DisplayMessage *msg)
{
    // TODO: Handle call ended - return to previous screen or show call ended message
    // screen_pop_page(); // Return to previous screen
}

static void handle_dialling(DisplayTaskContext *ctx, DisplayMessage *msg)
{
    char *caller_id = (char *)msg->data;
    if (caller_id)
    {
        // TODO: Create dialling page/overlay
        // Page *dialling_page = dialling_overlay_create(caller_id, NULL, NULL);
        // screen_push_page(dialling_page);
    }
}

static DisplayCmdHandler display_cmd_table[] = {
    [DISPLAY_HANDLE_INPUT] = handle_input_event,
    [DISPLAY_SET_PAGE] = handle_set_page,
    [DISPLAY_CLEAR_SCREEN] = handle_clear_screen,
    [DISPLAY_SET_SIGNAL_STATUS] = handle_set_signal_status,
    [DISPLAY_SET_BATTERY_STATUS] = handle_set_battery_status,
    [DISPLAY_SET_VOLUME] = handle_set_volume,
    [DISPLAY_INCOMING_CALL] = handle_incoming_call,
    [DISPLAY_ACTIVE_CALL] = handle_active_call,
    [DISPLAY_CALL_ENDED] = handle_call_ended,
    [DISPLAY_DIALLING] = handle_dialling,
};

static void dispatch_display_command(DisplayTaskContext *ctx, DisplayMessage *msg)
{
    if (msg->cmd < (sizeof(display_cmd_table) / sizeof(display_cmd_table[0])))
    {
        DisplayCmdHandler handler = display_cmd_table[msg->cmd];
        if (handler)
        {
            handler(ctx, msg);
        }
    }
}

static void display_task_main(void *pvParameters)
{
    DisplayTaskContext *ctx = (DisplayTaskContext *)pvParameters;
    DisplayMessage msg;

    // Initialize display subsystem inside the task context to avoid blocking kernel startup
    display_init();
    osDelay(100);
    display_fill(COLOUR_BLACK);
    theme_set_dark();
    draw_status_bar();
    status_bar_update_signal(5);
    status_bar_update_battery(50);
    screen_init(&menu_page);
    mark_all_tiles_dirty();
    screen_tick();

    // Task main loop - handles messages and ticks like the test file
    for (;;)
    {
        // Drain a burst of messages quickly to limit backlog, but cap per cycle
        int processed = 0;
        while (xQueueReceive(ctx->queue, &msg, 0))
        {
            dispatch_display_command(ctx, &msg);
            if (++processed >= 10)
            {
                break;
            }
        }
        // Update status bar and screen once per cycle
        status_bar_tick();
        screen_tick();
        // Always yield to other tasks - critical for system responsiveness
        osDelay(1);
    }
}

DisplayTaskContext *DisplayTask_Init(CallStateContext *call_ctx)
{
    static DisplayTaskContext display_ctx;
    memset(&display_ctx, 0, sizeof(display_ctx));

    // Store call state context for callbacks
    display_ctx.call_ctx = call_ctx;

    // Create queue
    display_ctx.queue = xQueueCreate(5, sizeof(DisplayMessage));
    if (!display_ctx.queue)
    {
        return NULL; // Failed to create queue
    }

    // Create and start the task
    osThreadAttr_t task_attr = {
        .name = "DisplayTask",
        .stack_size = DISPLAY_TASK_STACK_SIZE, // Increased stack size for display operations
        .priority = DISPLAY_TASK_PRIORITY};

    osThreadId_t thread_id = osThreadNew(display_task_main, &display_ctx, &task_attr);
    if (!thread_id)
    {
        vQueueDelete(display_ctx.queue);
        return NULL; // Failed to create task
    }

    return &display_ctx;
}

bool DisplayTask_PostCommand(DisplayTaskContext *ctx, DisplayCommand cmd, void *data)
{
    if (!ctx || !ctx->queue)
        return false;

    DisplayMessage msg = {
        .cmd = cmd,
        .data = data};
    return xQueueSend(ctx->queue, &msg, pdMS_TO_TICKS(10)) == pdTRUE;
}