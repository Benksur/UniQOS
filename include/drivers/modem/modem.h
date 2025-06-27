#ifndef MODEM_H_
#define MODEM_H_

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "errornum.h"
#include "stm32_config.h"
#include "at_commands.h"

uint8_t modem_init(void);

#endif