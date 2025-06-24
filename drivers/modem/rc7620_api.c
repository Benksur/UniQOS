#include "rc7620_api.h"
#include "stm32h7xx_hal.h"

static uint8_t curr_function_mode = 0;

uint8_t modem_write_command(const char *command)
{
    if (HAL_UART_Transmit(&MODEM_UART_HANDLE, (uint8_t *)command, strlen(command), 2000) != HAL_OK)
    {
        DEBUG_PRINTF("ERROR: Failed write to rc7620");
        return EIO;
    }
    return 0;
}

uint8_t modem_read_response(uint8_t *buffer, uint16_t max_len, uint32_t timeout)
{
    memset(buffer, 0, max_len);
    HAL_StatusTypeDef result = HAL_UART_Receive(&MODEM_UART_HANDLE, buffer, max_len - 1, timeout);

    if (result == HAL_TIMEOUT)
    {
        DEBUG_PRINTF("ERROR: Timed out reading response\r\n");
        return ETIMEDOUT;
    }
    else if (result != HAL_OK)
    {
        DEBUG_PRINTF("ERROR: Error reading response\r\n");
        return EIO;
    }

    return 0;
}

uint8_t modem_send_command(const char *command, char *response, uint16_t response_size, uint32_t read_timeout)
{
    char cmd_buffer[128];
    int cmd_len = snprintf(cmd_buffer, sizeof(cmd_buffer), "%s\r\n", command);
    uint8_t ret = 0;
    if (cmd_len < 0 || cmd_len >= sizeof(cmd_buffer))
    {
        DEBUG_PRINTF("ERROR: Bad Command Size\r\n");
        return E2BIG;
    }

    ret |= modem_write_command(cmd_buffer);
    if (ret)
    {
        return ret;
    }

    HAL_Delay(100);

    ret |= modem_read_response((uint8_t *)response, response_size, read_timeout);
    if (ret)
    {
        return ret;
    }

    return ret;
}

uint8_t modem_check_response_ok(const char *response)
{
    return (strstr(response, "OK") != NULL);
}

void modem_power_on(void)
{
    HAL_GPIO_WritePin(MODEM_POWER_PORT, MODEM_POWER_PIN, GPIO_PIN_RESET);
    HAL_Delay(250);
    HAL_GPIO_WritePin(MODEM_POWER_PORT, MODEM_POWER_PIN, GPIO_PIN_SET);
    HAL_Delay(100);
}

void modem_power_off(void)
{
    char response[32];
    modem_send_command("AT!POWERDOWN", response, sizeof(response), 2000);
    HAL_Delay(1000);
}
