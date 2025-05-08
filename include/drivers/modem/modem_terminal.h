#ifndef MODEM_TERMINAL_H
#define MODEM_TERMINAL_H

#include "stm32h7xx_hal.h"
#include "rc7620.h"
#include "usart.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void modem_terminal_run(void);
bool modem_terminal_test_uart_echo(void);
void modem_terminal_test(void);

#endif /* MODEM_TERMINAL_H */