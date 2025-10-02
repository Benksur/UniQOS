#include "power_task.h"
#include "bq27441.h"
#include <string.h>

struct PowerTaskContext
{
    QueueHandle_t queue;
    DisplayTaskContext *display_ctx;
    uint8_t last_soc;
};

typedef void (*PowerCmdHandler)(PowerTaskContext *ctx, PowerMessage *msg);

/* ===== HANDLERS ===== */
static void handle_init(PowerTaskContext *ctx, PowerMessage *msg)
{
    bq27441_init();
}

static void handle_get_soc(PowerTaskContext *ctx, PowerMessage *msg)
{
    uint8_t soc = bq27441_SOC();
    uint8_t *soc_ptr = (uint8_t *)msg->data;
    if (soc_ptr)
    {
        *soc_ptr = soc;
    }
}

static void handle_get_voltage(PowerTaskContext *ctx, PowerMessage *msg)
{
    uint16_t voltage = 0;
    bq27441_read_reg(BQ27441_CMD_VOLTAGE, &voltage);
    uint16_t *voltage_ptr = (uint16_t *)msg->data;
    if (voltage_ptr)
    {
        *voltage_ptr = voltage;
    }
}

static void handle_get_temperature(PowerTaskContext *ctx, PowerMessage *msg)
{
    uint16_t temperature = 0;
    bq27441_read_reg(BQ27441_CMD_INTERNAL_TEMPERATURE, &temperature);
    uint16_t *temp_ptr = (uint16_t *)msg->data;
    if (temp_ptr)
    {
        *temp_ptr = temperature;
    }
}

static void handle_shutdown(PowerTaskContext *ctx, PowerMessage *msg)
{
    // Send shutdown command to BQ27441
    uint16_t subcmd = BQ27441_CNTL_SHUTDOWN;
    bq27441_read_ctrl_reg(subcmd, &subcmd);
}

/* ===== DISPATCH TABLE ===== */
static PowerCmdHandler power_cmd_table[] = {
    [POWER_CMD_INIT] = handle_init,
    [POWER_CMD_GET_SOC] = handle_get_soc,
    [POWER_CMD_GET_VOLTAGE] = handle_get_voltage,
    [POWER_CMD_GET_TEMPERATURE] = handle_get_temperature,
    [POWER_CMD_SHUTDOWN] = handle_shutdown,
};

static void dispatch_power_command(PowerTaskContext *ctx, PowerMessage *msg)
{
    if (msg->cmd < (sizeof(power_cmd_table) / sizeof(power_cmd_table[0])))
    {
        PowerCmdHandler handler = power_cmd_table[msg->cmd];
        if (handler)
        {
            handler(ctx, msg);
        }
    }
}

static void power_task_main(void *pvParameters)
{
    PowerTaskContext *ctx = (PowerTaskContext *)pvParameters;
    PowerMessage msg;
    uint32_t last_battery_check = 0;

    // Initialize the battery fuel gauge
    bq27441_init();

    for (;;)
    {
        // Periodic battery check (every 5 seconds)
        if (HAL_GetTick() - last_battery_check >= 5000)
        {
            last_battery_check = HAL_GetTick();

            // Read current battery state of charge
            uint8_t current_soc = bq27441_SOC();

            // Only update display if battery level changed
            if (current_soc != ctx->last_soc)
            {
                ctx->last_soc = current_soc;
                if (ctx->display_ctx)
                {
                    DisplayTask_PostCommand(ctx->display_ctx, DISPLAY_SET_BATTERY_STATUS, &ctx->last_soc);
                }
            }
        }

        // Process any queued commands (non-blocking with short timeout)
        if (xQueueReceive(ctx->queue, &msg, pdMS_TO_TICKS(100)))
        {
            dispatch_power_command(ctx, &msg);
        }

        osDelay(100);
    }
}

PowerTaskContext *PowerTask_Init(DisplayTaskContext *display_ctx)
{
    static PowerTaskContext power_ctx;
    memset(&power_ctx, 0, sizeof(power_ctx));

    // Store display context for battery updates
    power_ctx.display_ctx = display_ctx;
    power_ctx.last_soc = 0;

    // Create queue for commands
    power_ctx.queue = xQueueCreate(5, sizeof(PowerMessage));
    if (!power_ctx.queue)
    {
        return NULL;
    }

    // Create and start the task
    osThreadAttr_t task_attr = {
        .name = "PowerTask",
        .stack_size = POWER_TASK_STACK_SIZE,
        .priority = POWER_TASK_PRIORITY};

    osThreadId_t thread_id = osThreadNew(power_task_main, &power_ctx, &task_attr);
    if (!thread_id)
    {
        vQueueDelete(power_ctx.queue);
        return NULL;
    }

    return &power_ctx;
}

bool PowerTask_PostCommand(PowerTaskContext *ctx, PowerCommand cmd, void *data)
{
    if (!ctx || !ctx->queue)
        return false;

    PowerMessage msg = {
        .cmd = cmd,
        .data = data};
    return xQueueSend(ctx->queue, &msg, 0) == pdTRUE;
}
