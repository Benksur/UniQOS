#include "cellular_task.h"

// Static task handle for ISR access
static TaskHandle_t g_cellular_task_handle = NULL;

// Convert dBm value to signal bars (0-5) based on signal strength ranges
static uint8_t dbm_to_signal_bars(int16_t dbm_value)
{
    if (dbm_value == 0 || dbm_value <= -93)
        return 0; // No service or very poor
    if (dbm_value <= -85)
        return 1; // Poor signal (1 bar)
    if (dbm_value <= -77)
        return 2; // Fair signal (2 bars)
    if (dbm_value <= -69)
        return 3; // Good signal (3 bars)
    if (dbm_value <= -61)
        return 4; // Very good signal (4 bars)
    return 5;     // Excellent signal (5 bars)
}

// Configure EXTI interrupt for RI pin (PC6)
static void configure_ri_interrupt(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Configure PC6 as interrupt on falling edge (pulse goes LOW)
    GPIO_InitStruct.Pin = MODEM_RI_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; // Trigger on falling edge
    GPIO_InitStruct.Pull = GPIO_PULLUP;          // Pull-up since active LOW
    HAL_GPIO_Init(MODEM_RI_PORT, &GPIO_InitStruct);

    // Enable EXTI6 interrupt with FreeRTOS-safe priority (6 or higher)
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

// GPIO EXTI callback - called from interrupt context
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == MODEM_RI_Pin && g_cellular_task_handle != NULL)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        // Notify the cellular task from ISR (lightweight, no RAM overhead)
        vTaskNotifyGiveFromISR(g_cellular_task_handle, &xHigherPriorityTaskWoken);

        // Yield if higher priority task was woken
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

struct CellularTaskContext
{
    QueueHandle_t queue;
    DisplayTaskContext *display_ctx;
    CallStateContext *call_ctx;
    uint8_t signal_bars;
};

typedef void (*CellularCmdHandler)(CellularTaskContext *ctx, CellularMessage *msg);

static void handle_init(CellularTaskContext *ctx, CellularMessage *msg)
{
    modem_init();
}

static void handle_sleep(CellularTaskContext *ctx, CellularMessage *msg)
{
    modem_sleep();
}

static void handle_airplane_mode_on(CellularTaskContext *ctx, CellularMessage *msg)
{
    modem_airplane_mode_on();
}

static void handle_airplane_mode_off(CellularTaskContext *ctx, CellularMessage *msg)
{
    modem_airplane_mode_off();
}

static void handle_send_sms(CellularTaskContext *ctx, CellularMessage *msg)
{
    if (!msg || !msg->data)
    {
        // TODO: Notify user of error
        return;
    }

    SmsData *sms_data = (SmsData *)msg->data;

    // Validate phone number and message are not empty
    if (sms_data->sms_address[0] == '\0' || sms_data->sms_message[0] == '\0')
    {
        // TODO: Notify user of invalid input
        return;
    }

    uint8_t result = modem_send_sms(sms_data->sms_address, sms_data->sms_message);

    if (result != 0)
    {
        // TODO: Post error to display task to show user SMS failed
        // DisplayTask_PostCommand(ctx->display_ctx, DISPLAY_SHOW_ERROR, "SMS Failed");
    }
    else
    {
        // TODO: Post success to display task
        // DisplayTask_PostCommand(ctx->display_ctx, DISPLAY_SHOW_SUCCESS, "SMS Sent");
    }
}

static void handle_dial(CellularTaskContext *ctx, CellularMessage *msg)
{
    modem_dial(msg->data);
    CallState_PostCommand(ctx->call_ctx, CALL_CMD_DIALLING, msg->data);
}

static void handle_hang_up(CellularTaskContext *ctx, CellularMessage *msg)
{
    modem_hang_up();
    CallState_PostCommand(ctx->call_ctx, CALL_CMD_HANGUP_CALL, NULL);
}

static CellularCmdHandler cellular_cmd_table[] = {
    [CELLULAR_CMD_INIT] = handle_init,
    [CELLULAR_CMD_SLEEP] = handle_sleep,
    [CELLULAR_CMD_AIRPLANE_MODE_ON] = handle_airplane_mode_on,
    [CELLULAR_CMD_AIRPLANE_MODE_OFF] = handle_airplane_mode_off,
    [CELLULAR_CMD_SEND_SMS] = handle_send_sms,
    [CELLULAR_CMD_DIAL] = handle_dial,
    [CELLULAR_CMD_HANG_UP] = handle_hang_up,
};

static void dispatch_cellular_command(CellularTaskContext *ctx, CellularMessage *msg)
{
    if (msg->cmd < (sizeof(cellular_cmd_table) / sizeof(cellular_cmd_table[0])))
    {
        CellularCmdHandler handler = cellular_cmd_table[msg->cmd];
        if (handler)
        {
            handler(ctx, msg);
        }
    }
}

static void cellular_task_main(void *pvParameters)
{
    CellularTaskContext *ctx = (CellularTaskContext *)pvParameters;
    CellularMessage msg;
    uint32_t last_signal_check = 0;

    // modem_init();

    for (;;)
    {
        // Wait for RI notification OR timeout (1 second for periodic signal check)
        // ulTaskNotifyTake: clears notification on exit, blocks with timeout
        uint32_t notification_value = ulTaskNotifyTake(
            pdTRUE,             // Clear notification value on exit
            pdMS_TO_TICKS(1000) // 1 second timeout
        );

        // Handle RI pulse notification (incoming call/SMS)
        if (notification_value > 0)
        {
            // RI pin pulsed LOW - check what type of event occurred
            char caller_id[32] = {0};
            uint8_t sms_index = 0;
            ModemEventType event = modem_check_event(caller_id, sizeof(caller_id), &sms_index);

            switch (event)
            {
            case MODEM_EVENT_INCOMING_CALL:
            {
                // Incoming call detected - notify call state task
                CallData call_data;
                if (caller_id[0] != '\0')
                {
                    strncpy(call_data.caller_id, caller_id, sizeof(call_data.caller_id) - 1);
                    call_data.caller_id[sizeof(call_data.caller_id) - 1] = '\0';
                }
                else
                {
                    strncpy(call_data.caller_id, "Unknown", sizeof(call_data.caller_id) - 1);
                }
                CallState_PostCommand(ctx->call_ctx, CALL_CMD_INCOMING_CALL, &call_data);
                break;
            }

            case MODEM_EVENT_INCOMING_SMS:
            {
                // Incoming SMS detected - retrieve and process
                // Static so data persists until display task processes it
                // Note: Will be overwritten if another SMS arrives before user views this one
                static ReceivedSmsData received_sms;
                memset(&received_sms, 0, sizeof(received_sms));

                if (modem_read_sms(sms_index, received_sms.sender, sizeof(received_sms.sender),
                                   received_sms.message, sizeof(received_sms.message)) == 0)
                {
                    // Successfully read SMS - pass full data to display task
                    // Display will show notification initially, full message when user opens
                    DEBUG_PRINTF("SMS from %s: %s\r\n", received_sms.sender, received_sms.message);
                    DisplayTask_PostCommand(ctx->display_ctx, DISPLAY_SHOW_SMS, &received_sms);
                }
                break;
            }

            case MODEM_EVENT_NONE:
            default:
                // No event or unknown event - ignore
                break;
            }
        }

        // Periodic signal strength check (every 2 seconds)
        if (HAL_GetTick() - last_signal_check >= 2000)
        {
            last_signal_check = HAL_GetTick();

            int16_t current_signal_dbm;
            uint8_t current_ber;
            uint8_t current_signal_bars;

            // Get signal strength in dBm from modem
            if (modem_get_signal_strength(&current_signal_dbm, &current_ber) == 0)
            {
                // Convert dBm value to signal bars (0-5)
                current_signal_bars = dbm_to_signal_bars(current_signal_dbm);

                if (current_signal_bars != ctx->signal_bars)
                {
                    ctx->signal_bars = current_signal_bars;
                    DisplayTask_PostCommand(ctx->display_ctx, DISPLAY_SET_SIGNAL_STATUS, &ctx->signal_bars);
                }
            }
        }

        // Process any queued commands (non-blocking)
        while (xQueueReceive(ctx->queue, &msg, 0))
        {
            dispatch_cellular_command(ctx, &msg);
        }
    }
}

CellularTaskContext *CellularTask_Init(DisplayTaskContext *display_ctx, CallStateContext *call_ctx)
{
    static CellularTaskContext cellular_ctx;
    memset(&cellular_ctx, 0, sizeof(cellular_ctx));

    // Create queue for commands
    cellular_ctx.queue = xQueueCreate(5, sizeof(CellularMessage));
    if (!cellular_ctx.queue)
    {
        return NULL;
    }

    cellular_ctx.display_ctx = display_ctx;
    cellular_ctx.call_ctx = call_ctx;
    cellular_ctx.signal_bars = 0;

    // Configure RI pin interrupt
    configure_ri_interrupt();

    osThreadAttr_t task_attr = {
        .name = "CellularTask",
        .stack_size = CELLULAR_TASK_STACK_SIZE,
        .priority = CELLULAR_TASK_PRIORITY};

    osThreadId_t thread_id = osThreadNew(cellular_task_main, &cellular_ctx, &task_attr);
    if (!thread_id)
    {
        vQueueDelete(cellular_ctx.queue);
        g_cellular_task_handle = NULL;
        return NULL;
    }

    // Store task handle in global for ISR access
    // Note: osThreadId_t is compatible with TaskHandle_t
    g_cellular_task_handle = (TaskHandle_t)thread_id;

    return &cellular_ctx;
}

bool CellularTask_PostCommand(CellularTaskContext *ctx, CellularCommand cmd, void *data)
{
    if (!ctx || !ctx->queue)
        return false;
    CellularMessage msg = {
        .cmd = cmd,
        .data = data};
    return xQueueSend(ctx->queue, &msg, 0) == pdTRUE;
}