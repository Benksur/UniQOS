#ifndef MCP73871_H
#define MCP73871_H

#include <stdint.h>

#define MCP73871_STAT1_PORT GPIOB
#define MCP73871_STAT1_PIN GPIO_PIN_15

#define MCP73871_STAT2_PORT GPIOB
#define MCP73871_STAT2_PIN GPIO_PIN_14

#define MCP73871_PG_PORT GPIOB
#define MCP73871_PG_PIN GPIO_PIN_13

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