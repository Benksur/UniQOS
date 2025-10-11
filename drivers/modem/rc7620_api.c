#include "rc7620_api.h"
#include "stm32h7xx_hal.h"

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
    uint16_t rxlen = 0;
    
    memset(buffer, 0, max_len);

    HAL_StatusTypeDef result = HAL_UARTEx_ReceiveToIdle(&MODEM_UART_HANDLE, buffer, max_len - 1, &rxlen, timeout);

    if (result == HAL_TIMEOUT && rxlen == 0)
    {
        DEBUG_PRINTF("ERROR: Timed out reading response\r\n");
        return ETIMEDOUT;
    }
    else if (result != HAL_OK && result != HAL_TIMEOUT)
    {
        DEBUG_PRINTF("ERROR: Error reading response\r\n");
        return EIO;
    }

    // probably need more sanity checking in here

    return 0;
}

// OLD version here in case receivetoidle has issues
uint8_t depr_modem_read_response(uint8_t *buffer, uint16_t max_len, uint32_t timeout)
{
    memset(buffer, 0, max_len);
    HAL_StatusTypeDef result = HAL_UART_Receive(&MODEM_UART_HANDLE, buffer, max_len - 1, timeout);

    if (result == HAL_TIMEOUT && buffer[0] == '\0')
    {
        DEBUG_PRINTF("ERROR: Timed out reading response\r\n");
        return ETIMEDOUT;
    }
    else if (result != HAL_OK && result != HAL_TIMEOUT)
    {
        DEBUG_PRINTF("ERROR: Error reading response\r\n");
        return EIO;
    }

    // probably need more sanity checking in here

    return 0;
}

uint8_t modem_send_command(const char *command, char *response, uint16_t response_size, uint32_t timeout)
{
    char cmd_buffer[128]; // may need to dynamically allocate
    int cmd_len = snprintf(cmd_buffer, sizeof(cmd_buffer), "%s\r\n", command);
    uint8_t ret = 0;
    uint32_t start_tick = HAL_GetTick();

    if (cmd_len < 0 || cmd_len >= sizeof(cmd_buffer))
    {
        DEBUG_PRINTF("ERROR: Bad Command Size\r\n");
        return E2BIG;
    }

    HAL_UART_Receive(&MODEM_UART_HANDLE, response, 100, 10);

    while (HAL_GetTick() - start_tick < timeout)
    {
        DEBUG_PRINTF("Writing AT command: %s\r\n", cmd_buffer);
        ret = modem_write_command(cmd_buffer);
        if (ret)
        {
            return ret;
        }

        ret = modem_read_response((uint8_t *)response, response_size, 100);
        if (ret == 0)
        {
            return ret;
            DEBUG_PRINTF("Got AT command response: %s\r\n", response);
        }
    }

    return ret;
}
uint8_t modem_check_response_ok(const char *response)
{
    //may want to adjust
    return (strstr(response, "\r\nOK\r\n") != NULL);
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

void modem_reset(void)
{
    HAL_GPIO_WritePin(MOB_RST_GPIO_Port, MOB_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(MOB_RST_GPIO_Port, MOB_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);    
}

uint8_t at_enter_module_pwd(char *password)
{
    char response[32];
    char cmd[32];
    uint8_t ret = 0;

    // set pwd and enter pwd require different lengths?
    if (strlen(password) < 4 || strlen(password) > 15)
    {
        DEBUG_PRINTF("Password wrong length");
        return EINVAL;
    }

    if (snprintf(cmd, sizeof(cmd), "AT!ENTERCND=\"%s\"", password) < 0)
    {
        DEBUG_PRINTF("ERROR: in creating string \"AT!ENTERCND=\"%s\"\"\r\n", password);
        return EINVAL;
    }

    ret |= modem_send_command(cmd, response, sizeof(response), 2000);

    if (ret || !modem_check_response_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    return ret;
}

uint8_t at_set_module_pwd(char *password)
{
    char response[32];
    char cmd[32];
    uint8_t ret = 0;

    if (strlen(password) < 8 || strlen(password) > 64)
    {
        DEBUG_PRINTF("Password wrong length");
        return EINVAL;
    }

    if (snprintf(cmd, sizeof(cmd), "AT!SETCND=\"%s\"", password) < 0)
    {
        DEBUG_PRINTF("ERROR: in creating string \"AT!SETCND=\"%s\"\"\r\n", password);
        return EINVAL;
    }

    ret |= modem_send_command(cmd, response, sizeof(response), 2000);

    if (ret || !modem_check_response_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    return ret;
}

uint8_t at_custom(char *customization, uint8_t value)
{
    char response[32];
    char cmd[32];
    uint8_t ret = 0;

    ret |= at_enter_module_pwd(RC7620_AT_PASSWORD);

    if (ret)
    {
        DEBUG_PRINTF("ERROR: password attempt failed");
        return EACCES;
    }

    if (snprintf(cmd, sizeof(cmd), "AT!CUSTOM=\"%s\",%d", customization, value) < 0)
    {
        DEBUG_PRINTF("ERROR: in creating string \"AT!CUSTOM=\"%s\",%d\"\r\n", customization, value);
        return EINVAL;
    }

    ret |= modem_send_command(cmd, response, sizeof(response), 30000);

    if (ret || !modem_check_response_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    return ret;
}
