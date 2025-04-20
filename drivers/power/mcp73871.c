
#include "mcp73871.h"

enum MCP73871_States mcp73871_status(void)
{
    GPIO_PinState stat1 = HAL_GPIO_ReadPin(MCP73871_STAT1_PORT, MCP73871_STAT1_PIN);
    GPIO_PinState stat2 = HAL_GPIO_ReadPin(MCP73871_STAT2_PORT, MCP73871_STAT2_PIN);
    GPIO_PinState pg = HAL_GPIO_ReadPin(MCP73871_PG_PORT, MCP73871_PG_PIN);

    enum MCP73871_States result = UNKNOWN;

    if (stat1 && stat2 && pg) {
        result = SHUTDOWN;
    } else if (stat1 && stat2 && !pg) {
        result = STANDBY;
    } else if (!stat1 && stat2 && pg) {
        result = LOW_BATTERY_OUT;
    } else if (!stat1 && stat2 && !pg) {
        result = CHARGING;
    } else if (!stat1 && !stat2 && !pg) {
        result = FAULT;
    } else if (stat1 && !stat2 && !pg) {
        result = CHARGE_COMPLETE;
    }

    return result
}