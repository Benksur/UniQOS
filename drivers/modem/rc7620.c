#include "rc7620.h"

uint8_t rc7620_write_command(const char *command)
{
    const uint32_t timeout = 2000;
    if (HAL_UART_Transmit(&MODEM_UART_HANDLE, (uint8_t *)command, strlen(command), timeout) != HAL_OK)
    {
        return 0;
    }
    return 1;
}


HAL_StatusTypeDef rc7620_read_response(uint8_t *buffer, uint16_t max_len, uint32_t timeout)
{
    memset(buffer, 0, max_len);
    return HAL_UART_Receive(&MODEM_UART_HANDLE, buffer, max_len - 1, timeout);
}

uint8_t rc7620_check_ok(const char *response)
{
    return (strstr(response, "OK") != NULL);
}

uint8_t rc7620_send_command(const char *command, char *response, uint16_t response_size, uint32_t read_timeout)
{
    char cmd_buffer[128]; 
    int cmd_len = snprintf(cmd_buffer, sizeof(cmd_buffer), "%s\r\n", command);
    if (cmd_len < 0 || cmd_len >= sizeof(cmd_buffer))
    {
        return 0;
    }

    if (!rc7620_write_command(cmd_buffer))
    {
        return 0;
    }

    HAL_Delay(100);

    if (rc7620_read_response((uint8_t *)response, response_size, read_timeout) != HAL_OK)
    {

        return 1;
    }

    return 1;
}

uint8_t rc7620_init(void)
{
    char response[128];
    const uint32_t default_timeout = 500;

    // test AT startup
    if (!rc7620_send_command("AT", response, sizeof(response), default_timeout) || !rc7620_check_ok(response))
    {
        return 0;
    }
    HAL_Delay(100);

    // set phone functionality 1 (full functionality, high power draw)
    if (!rc7620_send_command("AT+CFUN=1", response, sizeof(response), 5000) || !rc7620_check_ok(response))
    {
        return 0;
    }
    HAL_Delay(100);

    // check SIM status and/or needs pin
    if (!rc7620_send_command("AT+CPIN?", response, sizeof(response), default_timeout))
    {
        return 0; 
    }
    if (!strstr(response, "READY"))
    {
        return 0; 
    }
    HAL_Delay(100);

    return 1; // Initialization successful
}
