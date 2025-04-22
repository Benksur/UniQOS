#include "rc7620.h"

static uint8_t curr_function_mode = 0;

//
uint8_t rc7620_write_command(const char *command)
{
    const uint32_t timeout = 2000;
    if (HAL_UART_Transmit(&MODEM_UART_HANDLE, (uint8_t *)command, strlen(command), timeout) != HAL_OK)
    {
        DEBUG_PRINTF("ERROR: Failed write to rc7620");
        return EIO;
    }
    return 0;
}

uint8_t rc7620_read_response(uint8_t *buffer, uint16_t max_len, uint32_t timeout)
{
    memset(buffer, 0, max_len);
    HAL_StatusTypeDef result = HAL_UART_Receive(&MODEM_UART_HANDLE, buffer, max_len - 1, timeout);

    if (result == HAL_TIMEOUT) {
        DEBUG_PRINTF("ERROR: Timed out reading response");
        return ETIMEDOUT;
    } else if (result != HAL_OK){
        DEBUG_PRINTF("ERROR: Error reading response");
        return EIO;
    }

    return 0;
}

uint8_t rc7620_check_ok(const char *response)
{
    return (strstr(response, "OK") != NULL);
}

uint8_t rc7620_send_command(const char *command, char *response, uint16_t response_size, uint32_t read_timeout)
{
    char cmd_buffer[128];
    int cmd_len = snprintf(cmd_buffer, sizeof(cmd_buffer), "%s\r\n", command);
    uint8_t ret = 0;
    if (cmd_len < 0 || cmd_len >= sizeof(cmd_buffer))
    {
        DEBUG_PRINTF("ERROR: Bad Command Size");
        return E2BIG;
    }

    ret |= rc7620_write_command(cmd_buffer);
    if (ret)
    {
        return ret;
    }

    HAL_Delay(100);

    ret |= rc7620_read_response((uint8_t *)response, response_size, read_timeout);
    if (ret)
    {
        return ret;
    }

    return ret;
}

void rc7620_power_on(void)
{
    HAL_GPIO_WritePin(MODEM_POWER_PORT, MODEM_POWER_PIN, 0);
    HAL_Delay(250);
    HAL_GPIO_WritePin(MODEM_POWER_PORT, MODEM_POWER_PIN, 1);
    HAL_Delay(100);
}

void rc7620_power_off(void)
{
    char response[32];
    rc7620_send_command("AT!POWERDOWN", response, sizeof(response), 1000);
    HAL_Delay(1000);
}

uint8_t rc7620_set_function_mode(enum FunctionModes mode)
{
    char response[32];
    char cmd[10];

    // Only defined modes for RC7620
    if (mode != 0 && mode != 1 && mode != 4 && mode != 5 && mode != 6 && mode != 7)
    {
        DEBUG_PRINTF("ERROR: Bad function Mode");
        return EINVAL;
    }

    if (snprintf(cmd, sizeof(cmd), "AT+CFUN=%d", mode) < 0)
    {
        DEBUG_PRINTF("ERROR: in creating string \"AT+CFUN=%d\"\r\n", mode);
        return EINVAL;
    }

    if (rc7620_send_command(cmd, response, sizeof(response), 3000) || !rc7620_check_ok(response)) // not sure this will actually respond with OK but ig we can see
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    curr_function_mode = mode;

    return 0;
}

uint8_t rc7620_toggle_airplane_mode(void)
{
    if (curr_function_mode == 4)
    {
        return rc7620_set_function_mode(MODE_FULL);
    }
    else
    {
        return rc7620_set_function_mode(MODE_AIRPLANE);
    }
}

uint8_t rc7620_init(void)
{
    char response[128];
    const uint32_t default_timeout = 500;
    uint8_t ret = 0;

    // test AT startup
    DEBUG_PRINTF("Sending: AT\r\n");
    ret |= rc7620_send_command("AT", response, sizeof(response), default_timeout);
    if (ret || !rc7620_check_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return ret;
    }
    DEBUG_PRINTF("Response: %s\r\n", response);
    HAL_Delay(100);

    // disable echo
    DEBUG_PRINTF("Sending: ATE0\r\n");
    rc7620_send_command("ATE0", response, sizeof(response), default_timeout);
    DEBUG_PRINTF("Response: %s\r\n", response);
    HAL_Delay(100);

    // disable GPS
    DEBUG_PRINTF("Sending: AT!CUSTOM=\"GPSENABLE\",0\r\n");
    rc7620_send_command("AT!CUSTOM=\"GPSENABLE\",0", response, sizeof(response), default_timeout);
    DEBUG_PRINTF("Response: %s\r\n", response);
    HAL_Delay(100);

    // check SIM status and/or needs pin
    DEBUG_PRINTF("Sending: AT+CPIN?\r\n");
    ret |= rc7620_send_command("AT+CPIN?", response, sizeof(response), default_timeout);
    if (ret)
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return ret;
    }
    DEBUG_PRINTF("Response: %s\r\n", response);
    if (!strstr(response, "READY"))
    {
        return EBUSY;
    }
    HAL_Delay(100);

    // AT+IPR= to change baud rate if needed, default 115200

    // print IMEI number (not needed but just for debugging)
    DEBUG_PRINTF("Sending: AT+GSN\r\n");
    rc7620_send_command("AT+GSN", response, sizeof(response), default_timeout);
    DEBUG_PRINTF("Response: %s\r\n", response);
    HAL_Delay(100);

    // check network registration status
    DEBUG_PRINTF("Sending: AT+CREG?\r\n");
    rc7620_send_command("AT+CREG?", response, sizeof(response), default_timeout);
    DEBUG_PRINTF("Response: %s\r\n", response);
    HAL_Delay(100);

    DEBUG_PRINTF("Sending: AT+CEREG?\r\n");
    rc7620_send_command("AT+CEREG?", response, sizeof(response), default_timeout);
    DEBUG_PRINTF("Response: %s\r\n", response);
    HAL_Delay(100);

    // set phone functionality 1 (full functionality, high power draw)
    DEBUG_PRINTF("Sending: AT+CFUN=1\r\n");
    ret |= rc7620_send_command("AT+CFUN=1", response, sizeof(response), 5000);
    if (ret || !rc7620_check_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return 0;
    }
    DEBUG_PRINTF("Response: %s\r\n", response);
    HAL_Delay(100);

    // Set APN to telstra IP
    DEBUG_PRINTF("Sending: AT+CGDCONT=1,\"IP\",\"telstra.internet\"\r\n");
    rc7620_send_command("AT+CGDCONT=1,\"IP\",\"telstra.internet\"", response, sizeof(response), default_timeout);
    DEBUG_PRINTF("Response: %s\r\n", response);
    HAL_Delay(100);

    // notify on new sms
    DEBUG_PRINTF("Sending: AT+CNMI=1,1,0,0,0\r\n");
    rc7620_send_command("AT+CNMI=1,1,0,0,0", response, sizeof(response), default_timeout);
    DEBUG_PRINTF("Response: %s\r\n", response);
    HAL_Delay(100);
    return ret; // Initialization successful
}

uint8_t rc7620_display_sms(int index)
{
    char command_buffer[32];
    char response_buffer[256]; // Internal buffer
    uint32_t timeout_ms = 3000;
    uint8_t ret = 0;

    memset(response_buffer, 0, sizeof(response_buffer));

    snprintf(command_buffer, sizeof(command_buffer), "AT+CMGR=%d", index);
    DEBUG_PRINTF("Requesting SMS display for index %d: %s\\r\\n", index, command_buffer);

    if (rc7620_send_command(command_buffer, response_buffer, sizeof(response_buffer), timeout_ms))
    {
        if (strstr(response_buffer, "+CMGR:") != NULL)
        {
            // response in form +CMGR: <status>,<sender_address>,[<alpha>],<timestamp>[,<optional_fields>]<CR><LF>
            // <message_body><CR><LF>
            // <CR><LF>
            // OK<CR><LF>
            DEBUG_PRINTF("--- SMS Index %d ---\\r\\n%s\\r\\n--- End SMS ---\\r\\n", index, response_buffer);
            ret = EBADMSG;
        }
        else
        {
            DEBUG_PRINTF("Failed to display SMS content at index %d (Invalid response/Timeout):\\r\\n%s\\r\\n", index, response_buffer);
        }
    }
    else
    {
        DEBUG_PRINTF("Failed to send AT+CMGR command for index %d.\\r\\n", index);
    }

    return ret;
}

uint8_t rc7620_send_sms(const char *sms_address, const char *sms_message)
{
    char command_buffer[160];
    char response_buffer[128];
    uint32_t sms_timeout_ms = 30000;
    uint8_t ret = 0;

    DEBUG_PRINTF("Setting SMS text mode: AT+CMGF=1\r\n");
    ret |= rc7620_send_command("AT+CMGF=1", response_buffer, sizeof(response_buffer), 1000);
    if (ret || !rc7620_check_ok(response_buffer))
    {
        DEBUG_PRINTF("Failed to set SMS mode to text. Response:\r\n%s\r\n", response_buffer);
        return EBADMSG;
    }
    DEBUG_PRINTF("SMS mode set. Response:\r\n%s\r\n", response_buffer);
    HAL_Delay(100);

    int cmd_len = snprintf(command_buffer, sizeof(command_buffer), "AT+CMGS=\"%s\"\r\n", sms_address);
    if (cmd_len < 0 || cmd_len >= sizeof(command_buffer))
    {
        DEBUG_PRINTF("Failed to format AT+CMGS command (number too long?).\r\n");
        return E2BIG;
    }

    DEBUG_PRINTF("Sending SMS command: %s", command_buffer);
    ret |= rc7620_write_command(command_buffer);
    if (ret)
    {
        DEBUG_PRINTF("Failed to write AT+CMGS command.\r\n");
        return ret;
    }

    HAL_Delay(100);
    memset(response_buffer, 0, sizeof(response_buffer));
    ret |= rc7620_read_response((uint8_t *)response_buffer, sizeof(response_buffer), 1000);
    if (ret)
    {
        DEBUG_PRINTF("Did not receive prompt > within timeout.\r\n");
        return ret;
    }

    char *prompt_start = response_buffer;
    while (*prompt_start == '\r' || *prompt_start == '\n' || *prompt_start == ' ')
    {
        prompt_start++;
    }

    if (strncmp(prompt_start, "> ", 2) != 0)
    {
        DEBUG_PRINTF("Did not receive correct prompt >. Received:\r\n%s\r\n", response_buffer);
        return EBADMSG;
    }
    DEBUG_PRINTF("Received prompt >\r\n");

    DEBUG_PRINTF("Sending message body: %s\r\n", sms_message);
    ret |= rc7620_write_command(sms_message);
    if (ret)
    {
        DEBUG_PRINTF("Failed to write SMS message body.\r\n");
        return ret;
    }

    DEBUG_PRINTF("Sending Ctrl+Z (0x1A)\r\n");
    char ctrl_z[2] = {0x1A, 0x00};
    ret |= rc7620_write_command(ctrl_z);
    if (ret)
    {
        DEBUG_PRINTF("Failed to write Ctrl+Z.\r\n");
        return ret;
    }

    memset(response_buffer, 0, sizeof(response_buffer));
    ret |= rc7620_read_response((uint8_t *)response_buffer, sizeof(response_buffer), sms_timeout_ms);
    if (ret)
    {
        DEBUG_PRINTF("Did not receive final response within timeout.\r\n");
        return ret;
    }

    DEBUG_PRINTF("Final response:\r\n%s\r\n", response_buffer);

    if (strstr(response_buffer, "+CMGS") != NULL && rc7620_check_ok(response_buffer))
    {
        DEBUG_PRINTF("SMS sent successfully.\r\n");
        return 0;
    }
    else
    {
        DEBUG_PRINTF("SMS send final response did not contain +CMGS and OK.\r\n");
        return EBADMSG;
    }
}
