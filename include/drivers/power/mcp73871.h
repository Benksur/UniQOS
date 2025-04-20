#ifndef MCP73871_H
#define MCP73871_H

#include <stdint.h>
#include "stm32h7xx_hal.h"
#include "stm32_config.h"


enum MCP73871_States {
    UNKNOWN = 0, //Non defined state
    SHUTDOWN, 
    STANDBY,
    LOW_BATTERY_OUT,
    CHARGING,
    FAULT,
    CHARGE_COMPLETE,
};

enum MCP73871_States mcp73871_status(void);

#endif