#include "display_task.h"
#include "call_state.h"
#include "cellular_task.h"
#include "power_task.h"
#include "incoming_text.h"
#include "messages.h"
#include "sms_types.h"
#include <string.h>

struct DisplayTaskContext
{
    QueueHandle_t queue;
    CallStateContext *call_ctx;        // Reference to call state for callbacks
    CellularTaskContext *cellular_ctx; // Reference to cellular task for callbacks
    PowerTaskContext *power_ctx;
};

typedef void (*DisplayCmdHandler)(DisplayTaskContext *ctx, DisplayMessage *msg);

/* ===== CALLBACK FOR INCOMING CALL OVERLAY ===== */
static void incoming_call_callback(int action, void *user_data)
{
    DisplayTaskContext *ctx = (DisplayTaskContext *)user_data;
    if (!ctx)
        return;

    switch (action)
    {
    case INCOMING_CALL_ACTION_PICKUP:
        // Answer the call
        CallState_PostCommand(ctx->call_ctx, CALL_CMD_ANSWER_CALL, NULL);
        break;
    case INCOMING_CALL_ACTION_HANGUP:
        // Reject/hangup the call
        CallState_PostCommand(ctx->call_ctx, CALL_CMD_HANGUP_CALL, NULL);
        break;
    default:
        break;
    }
}

/* ===== CALLBACK FOR INCOMING SMS OVERLAY ===== */
static void incoming_text_callback(int action, void *user_data)
{
    ReceivedSms *sms_data = (ReceivedSms *)user_data;
    if (!sms_data)
        return;

    switch (action)
    {
    case INCOMING_TEXT_ACTION_OPEN:
        // User wants to open and read the full SMS
        MessagePageState state;
        strncpy(state.sender, sms_data->sender, sizeof(state.sender) - 1);
        state.sender[sizeof(state.sender) - 1] = '\0';
        strncpy(state.message, sms_data->body, sizeof(state.message) - 1);
        state.message[sizeof(state.message) - 1] = '\0';

        Page *sms_page = messages_page_create(state);
        screen_set_page(sms_page);
        break;
    case INCOMING_TEXT_ACTION_CLOSE:
        screen_pop_page();
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
            // Only allow popping page if not currently in a call
            CallState current_state = CALL_STATE_IDLE;
            if (ctx->call_ctx)
            {
                current_state = CallState_GetCurrentState(ctx->call_ctx);
            }

            // Prohibit popping page if in an active call (ringing, dialing, or active)
            if (current_state == CALL_STATE_IDLE)
            {
                screen_pop_page();
            }
            // If in a call, ignore the INPUT_RIGHT button press
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
        Page *incoming_call_page = incoming_call_overlay_create(caller_id, incoming_call_callback, ctx);
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

static void handle_show_sms(DisplayTaskContext *ctx, DisplayMessage *msg)
{
    ReceivedSms *sms_data = (ReceivedSms *)msg->data;
    if (sms_data)
    {
        // Show SMS notification overlay with sender
        // The full message is passed as user_data for when user opens it
        Page *sms_notification = incoming_text_overlay_create(sms_data->sender,
                                                              incoming_text_callback,
                                                              sms_data);
        screen_push_page(sms_notification);
    }
}

static void handle_set_battery_page(DisplayTaskContext *ctx, DisplayMessage *msg)
{

    screen_handle_response(PAGE_RESPONSE_BATTERY_HC, msg->data);
}

static void handle_sync_rtc(DisplayTaskContext *ctx, DisplayMessage *msg)
{
    RtcSyncData *rtc_data = (RtcSyncData *)msg->data;
    if (rtc_data)
    {
        HAL_RTC_SetTime(&hrtc, &rtc_data->time, RTC_FORMAT_BIN);
        HAL_RTC_SetDate(&hrtc, &rtc_data->date, RTC_FORMAT_BIN);
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
    [DISPLAY_SHOW_SMS] = handle_show_sms,
    [DISPLAY_SET_BATTERY_PAGE] = handle_set_battery_page,
    [DISPLAY_SYNC_RTC] = handle_sync_rtc,
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

    // turn backlight full power
    HAL_GPIO_WritePin(LOAD_SW_GPIO_Port, LOAD_SW_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET); // backlight

    // Task main loop - handles messages and ticks like the test file
    for (;;)
    {
        // Drain a burst of messages quickly to limit backlog, but cap per cycle
        int processed = 0;
        while (xQueueReceive(ctx->queue, &msg, 0))
        {
            dispatch_display_command(ctx, &msg);
            if (++processed >= 5)
            {
                break;
            }
        }

        // Check for pending requests from screen module
        int request_type;
        void *request_data;
        if (screen_get_pending_request(&request_type, &request_data))
        {
            // Handle the request by forwarding to appropriate task
            switch (request_type)
            {
            case PAGE_REQUEST_HANGUP_CALL:
                if (ctx->cellular_ctx)
                {
                    CellularTask_PostCommand(ctx->cellular_ctx, CELLULAR_CMD_HANG_UP, NULL);
                }
                break;
            case PAGE_REQUEST_MAKE_CALL:
            {
                char *phone_number = (char *)request_data;
                if (phone_number && ctx->cellular_ctx)
                {
                    // Post dial command to cellular task
                    CellularTask_PostCommand(ctx->cellular_ctx, CELLULAR_CMD_DIAL, phone_number);
                }
            }
            break;
            case PAGE_REQUEST_SMS_SEND:
            {
                SmsMessage *sms_data = (SmsMessage *)request_data;
                if (sms_data && ctx->cellular_ctx)
                {
                    // Validate phone number and message are not empty
                    if (sms_data->recipient[0] != '\0' && sms_data->body[0] != '\0')
                    {
                        // Post SMS send command to cellular task
                        CellularTask_PostCommand(ctx->cellular_ctx, CELLULAR_CMD_SEND_SMS, sms_data);
                    }
                }
            }
            break;
            case PAGE_REQUEST_BATTERY_HC:
            {
                PowerTask_PostCommand(ctx->power_ctx, POWER_CMD_STATS, NULL);
            }
            default:
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

DisplayTaskContext *DisplayTask_Init(CallStateContext *call_ctx, CellularTaskContext *cellular_ctx)
{
    static DisplayTaskContext display_ctx;
    memset(&display_ctx, 0, sizeof(display_ctx));

    // Store call state and cellular contexts for callbacks
    display_ctx.call_ctx = call_ctx;
    display_ctx.cellular_ctx = cellular_ctx;

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

void DisplayTask_SetCellularContext(DisplayTaskContext *ctx, CellularTaskContext *cellular_ctx)
{
    if (ctx)
    {
        ctx->cellular_ctx = cellular_ctx;
    }
}

void DisplayTask_SetPowerContext(DisplayTaskContext *ctx, PowerTaskContext *power_ctx)
{
    if (ctx)
    {
        ctx->power_ctx = power_ctx;
    }
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
