#include "cellular_task.h"

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
    return 5; // Excellent signal (5 bars)
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
    modem_send_sms(msg->data);
}

static void handle_dial(CellularTaskContext *ctx, CellularMessage *msg)
{
    modem_dial(msg->data);
    CallState_PostCommand(ctx->call_ctx, CALL_CMD_DIALLING, msg->data);

}

static CellularCmdHandler cellular_cmd_table[] = {
    [CELLULAR_CMD_INIT] = handle_init,
    [CELLULAR_CMD_SLEEP] = handle_sleep,
    [CELLULAR_CMD_AIRPLANE_MODE_ON] = handle_airplane_mode_on,
    [CELLULAR_CMD_AIRPLANE_MODE_OFF] = handle_airplane_mode_off,
    [CELLULAR_CMD_SEND_SMS] = handle_send_sms,
    [CELLULAR_CMD_DIAL] = handle_dial,
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
    modem_init();
    for (;;)
    {
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

        // check received sms here
        modem_check_received_sms();
        
        if (xQueueReceive(ctx->queue, &msg, 0))
        {
            dispatch_cellular_command(ctx, &msg);
        }
    }
}

CellularTaskContext *CellularTask_Init(DisplayTaskContext *display_ctx, CallStateContext *call_ctx)
{
    static CellularTaskContext cellular_ctx;
    memset(&cellular_ctx, 0, sizeof(cellular_ctx));
    cellular_ctx.queue = xQueueCreate(5, sizeof(CellularMessage));
    cellular_ctx.display_ctx = display_ctx;
    cellular_ctx.call_ctx = call_ctx;
    cellular_ctx.signal_bars = 0;

    osThreadAttr_t task_attr = {
        .name = "CellularTask",
        .stack_size = CELLULAR_TASK_STACK_SIZE,
        .priority = CELLULAR_TASK_PRIORITY};
    osThreadNew(cellular_task_main, &cellular_ctx, &task_attr);


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