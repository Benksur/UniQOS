#include "modem_terminal.h"
#include "stm32_config.h"
#include "rc7620.h"
#include <stdio.h>
#include <string.h>

#define INPUT_BUFFER_SIZE 128
#define RESPONSE_BUFFER_SIZE 256

// redirect printf to DEBUG_UART_HANDLE
int __io_putchar(int ch)
{
    HAL_UART_Transmit(&DEBUG_UART_HANDLE, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

HAL_StatusTypeDef read_line_from_debug_uart(char *buffer, uint16_t max_len)
{
    uint16_t index = 0;
    uint8_t received_char;
    HAL_StatusTypeDef status;

    memset(buffer, 0, max_len);

    while (index < max_len - 1)
    {
        status = HAL_UART_Receive(&DEBUG_UART_HANDLE, &received_char, 1, HAL_MAX_DELAY);
        if (status != HAL_OK)
        {
            return status;
        }

        HAL_UART_Transmit(&DEBUG_UART_HANDLE, &received_char, 1, HAL_MAX_DELAY);

        if (received_char == '\r' || received_char == '\n')
        {
            buffer[index] = '\0';
            DEBUG_PRINTF("\r\n");
            return HAL_OK;
        }
        else if (received_char == '\b' || received_char == 127)
        {
            if (index > 0)
            {
                index--;
                DEBUG_PRINTF("\b \b");
            }
        }
        else if (received_char >= ' ' && received_char <= '~')
        {
            buffer[index++] = received_char;
        }
    }

    buffer[index] = '\0';
    return HAL_OK;
}

void modem_terminal_run(void)
{
    char input_buffer[INPUT_BUFFER_SIZE];
    char response_buffer[RESPONSE_BUFFER_SIZE];

    printf("\r\n--- RC7620 AT Command Terminal ---\r\n");

    DEBUG_PRINTF("Initializing RC7620...\r\n");
    if (rc7620_init())
    {
        DEBUG_PRINTF("Modem initialized successfully.\r\n");
    }
    else
    {
        DEBUG_PRINTF("Modem initialization FAILED. Proceeding to terminal anyway...\r\n");
    }

    printf("Enter AT commands (or type 'exit' to quit):\r\n");

    while (1)
    {
        printf("> ");
        fflush(stdout);

        if (read_line_from_debug_uart(input_buffer, INPUT_BUFFER_SIZE) != HAL_OK)
        {
            DEBUG_PRINTF("Error reading input.\r\n");
            continue;
        }

        if (strlen(input_buffer) == 0)
        {
            continue;
        }

        if (strcmp(input_buffer, "exit") == 0)
        {
            printf("Exiting modem terminal.\r\n");
            break;
        }

        DEBUG_PRINTF("Sending: %s\r\n", input_buffer);
        if (!rc7620_send_command(input_buffer, response_buffer, RESPONSE_BUFFER_SIZE, 2000))
        {
            DEBUG_PRINTF("Failed to send command (write error or buffer overflow).\r\n");
        }
        else
        {
            DEBUG_PRINTF("Response:\r\n%s\r\n", response_buffer);
        }
    }
}