#include "modem_terminal.h"

#define INPUT_BUFFER_SIZE 128
#define RESPONSE_BUFFER_SIZE 256

// redirect printf to DEBUG_UART_HANDLE
int __io_putchar(int ch)
{
    HAL_UART_Transmit(&DEBUG_UART_HANDLE, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}

// redirect getchar to DEBUG_UART_HANDLE
int __io_getchar(void)
{
    uint8_t ch;
    HAL_UART_Receive(&DEBUG_UART_HANDLE, &ch, 1, HAL_MAX_DELAY);
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
    if (modem_init())
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

        // Trim any trailing \r\n that might be in the input buffer
        int len = strlen(input_buffer);
        while (len > 0 && (input_buffer[len-1] == '\r' || input_buffer[len-1] == '\n')) {
            input_buffer[len-1] = '\0';
            len--;
        }

        DEBUG_PRINTF("Sending: %s\r\n", input_buffer);
        if (!modem_send_command(input_buffer, response_buffer, RESPONSE_BUFFER_SIZE, 2000))
        {
            DEBUG_PRINTF("Failed to send command (write error or buffer overflow).\r\n");
        }
        else
        {
            DEBUG_PRINTF("Response:\r\n%s\r\n", response_buffer);
        }
    }
}

bool modem_terminal_test_uart_echo(void)
{
    char test_message[] = "Hello Nucleo!\r\n";
    char response_buffer[RESPONSE_BUFFER_SIZE];
    uint16_t index = 0;
    int received_char;
    
    DEBUG_PRINTF("Testing UART terminal echo...\r\n");
    
    // Send test message
    for (uint16_t i = 0; i < strlen(test_message); i++)
    {
        HAL_UART_Transmit(&DEBUG_UART_HANDLE, (uint8_t *)&test_message[i], 1, HAL_MAX_DELAY);
    }
    
    // Wait for echo response
    while (index < RESPONSE_BUFFER_SIZE - 1)
    {
        received_char = __io_getchar();
        DEBUG_PRINTF("Received char: %c\r\n", received_char);
        // if (received_char == EOF)
        // {
        //     DEBUG_PRINTF("Test failed: Error reading response\r\n");
        //     return false;
        // }
        
        // response_buffer[index++] = (char)received_char;
        
        // // Check if we received the complete message
        // if (index >= strlen(test_message))
        // {
        //     response_buffer[index] = '\0';
        //     if (strncmp(response_buffer, test_message, strlen(test_message)) == 0)
        //     {
        //         DEBUG_PRINTF("Test successful! Echo received: %s\r\n", response_buffer);
        //         return true;
        //     }
        // }
    }
    
    DEBUG_PRINTF("Test failed: Buffer overflow or incorrect response\r\n");
    return false;
}

void modem_terminal_test(void) {
    char input_buffer[INPUT_BUFFER_SIZE];
    char response_buffer[RESPONSE_BUFFER_SIZE];
    uint16_t index = 0;
    int received_char;
    
    DEBUG_PRINTF("Enter AT commands (type 'exit' to quit):\r\n");
    
    while (1)
    {
            received_char = __io_getchar();
            // Echo the character directly using UART
            HAL_UART_Transmit(&DEBUG_UART_HANDLE, (uint8_t *)&received_char, 1, HAL_MAX_DELAY);
            
            if (received_char == '\r' || received_char == '\n')
            {
                input_buffer[index] = '\0';
                DEBUG_PRINTF("\r\n");
                
                if (index > 0)
                {
                    if (strcmp(input_buffer, "exit") == 0)
                    {
                        return;
                    }
                    
                    // int len = strlen(input_buffer);
                    // while (len > 0 && (input_buffer[len-1] == '\r' || input_buffer[len-1] == '\n')) {
                    //     input_buffer[len-1] = '\0';
                    //     len--;
                    // }
                    
                    if (modem_send_command(input_buffer, response_buffer, RESPONSE_BUFFER_SIZE, 10000) != 0)
                    {
                        DEBUG_PRINTF("Failed to send command\r\n");
                    }
                    else
                    {
                        DEBUG_PRINTF("Response: %s\r\n", response_buffer);
                    }
                }
                index = 0;  // Reset index for next command
                continue;   // Continue to next iteration instead of breaking
            }
            input_buffer[index++] = received_char;
    }
}