/**
 * @file modem_terminal.h
 * @brief Modem terminal interface for debugging
 * @ingroup modem_drivers
 *
 * Provides terminal-based interface for debugging and testing modem
 * functionality. Allows direct AT command interaction and UART testing.
 */

#ifndef MODEM_TERMINAL_H
#define MODEM_TERMINAL_H

#include "stm32h7xx_hal.h"
#include "modem.h"
#include "usart.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

/**
 * @ingroup modem_drivers
 * @brief Run the modem terminal interface
 *
 * Starts the interactive terminal interface for modem debugging.
 * This function blocks and provides a command-line interface for
 * testing modem functionality.
 */
void modem_terminal_run(void);

/**
 * @ingroup modem_drivers
 * @brief Test UART echo functionality
 * @return true if echo test passes, false otherwise
 *
 * Tests the UART communication by sending a test message and
 * verifying the echo response from the modem.
 */
bool modem_terminal_test_uart_echo(void);

/**
 * @ingroup modem_drivers
 * @brief Run comprehensive modem tests
 *
 * Executes a series of tests to verify modem functionality including
 * initialization, communication, and basic operations.
 */
void modem_terminal_test(void);

#endif /* MODEM_TERMINAL_H */