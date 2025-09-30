#include "cellular_task.h"

struct CellularTaskContext
{
    QueueHandle_t queue;
    DisplayTaskContext *display_ctx;
    CallStateContext *call_ctx;
    uint8_t signal_strength;
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
    for (;;)
    {
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
    cellular_ctx.signal_strength = 0;

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