#include "rc7620.h"
#include "stm32h7xx_hal.h"
static uint8_t curr_function_mode = 0;

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
        DEBUG_PRINTF("ERROR: Bad Command Size\r\n");
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
    HAL_GPIO_WritePin(MODEM_POWER_PORT, MODEM_POWER_PIN, GPIO_PIN_RESET);
    HAL_Delay(250);
    HAL_GPIO_WritePin(MODEM_POWER_PORT, MODEM_POWER_PIN, GPIO_PIN_SET);
    HAL_Delay(100);
}

void rc7620_power_off(void)
{
    char response[32];
    rc7620_send_command("AT!POWERDOWN", response, sizeof(response), TIMEOUT_2S);
    HAL_Delay(1000);
}

uint8_t rc7620_set_function_mode(enum FunctionModes mode)
{
    char response[32];
    char cmd[10];

    // Only defined modes for RC7620
    if (mode != 0 && mode != 1 && mode != 4 && mode != 5 && mode != 6 && mode != 7)
    {
        DEBUG_PRINTF("ERROR: Bad function Mode\r\n");
        return EINVAL;
    }

    if (snprintf(cmd, sizeof(cmd), "AT+CFUN=%d", mode) < 0)
    {
        DEBUG_PRINTF("ERROR: in creating string \"AT+CFUN=%d\"\r\n", mode);
        return EINVAL;
    }

    if (rc7620_send_command(cmd, response, sizeof(response), TIMEOUT_30S) || !rc7620_check_ok(response)) // not sure this will actually respond with OK but ig we can see
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

uint8_t rc7620_get_clock(enum FunctionModes mode, RTC_DateTypeDef *date, RTC_TimeTypeDef *time)
{
    char response[100];
    uint8_t ret = 0;
    int matches = 0;
    int8_t utcoffset;

    RTC_DateTypeDef datestructure;
    RTC_TimeTypeDef timestructure;

    ret |= rc7620_send_command("AT+CCLK?", response, sizeof(response), TIMEOUT_30S) || !rc7620_check_ok(response);

    if (ret || !rc7620_check_ok(response)) // not sure this will actually respond with OK but ig we can see
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    // response should be in form "+CCLK: yy/MM/dd,hh:mm:ss±zz"
    matches = sscanf(response, "+CCLK: %hhd/%hhd/%hhd,%hhd:%hhd:%hhd%hhd",
                     &datestructure.Year, &datestructure.Month, &datestructure.Date,
                     &timestructure.Hours, &timestructure.Minutes, &timestructure.Seconds,
                     &utcoffset);

    if (matches != 7)
    {
        DEBUG_PRINTF("Bad Matches on Response: %s\r\n", response);
        return EBADMSG;
    }

    memcpy(date, &datestructure, sizeof(RTC_DateTypeDef));
    memcpy(time, &timestructure, sizeof(RTC_TimeTypeDef));

    return ret;
}

uint8_t rc7620_signal_strength(int16_t *rssi, uint8_t *ber)
{
    uint8_t ret = 0;
    char response[100];
    int matches = 0;
    uint8_t rssi_val, ber_val;
    int16_t rssi_val_db = 0; // unknown val

    ret |= rc7620_send_command("AT+CSQ", response, sizeof(response), TIMEOUT_2S);

    if (ret || !rc7620_check_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    // response in form +CSQ: <rssi>,<ber>
    matches = sscanf(response, "+CSQ: %hhd,%hhd",
                     &rssi_val, &ber_val);

    if (matches != 2)
    {
        DEBUG_PRINTF("Bad Matches on Response: %s\r\n", response);
        return EBADMSG;
    }

    if (rssi_val != 99)
    {
        rssi_val_db = (rssi_val * 2) - 113; // to db
    }
    else
    {
        rssi_val_db = 0;
    }

    memcpy(rssi, &rssi_val_db, sizeof(int16_t));
    memcpy(ber, &ber_val, sizeof(uint8_t));

    return ret;
}

uint8_t rc7620_select_char_set(char *chset)
{
    char response[100];
    char cmd[24];
    uint8_t ret = 0;

    // 6 Char max length for 3GPP TS 27.007 Sec 5.5
    if (strlen(chset) > 6)
    {
        return EINVAL;
    }

    //+CSCS=[<chset>]
    if (snprintf(cmd, sizeof(cmd), "AT+CSCS=\"%s\"", chset) < 0)
    {
        chset
            DEBUG_PRINTF("ERROR: in creating string \"AT+CSCS=\"%s\"\"\r\n", chset);
        return EINVAL;
    }

    ret |= rc7620_send_command(cmd, response, sizeof(response), TIMEOUT_2S);

    if (ret || !rc7620_check_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    return ret;
}

uint8_t rc7620_select_phonebook_memory(char storage[2], char *password)
{
    char response[100];
    char cmd[24];
    uint8_t ret = 0;

    if (strlen(storage) != 2)
    {
        return EINVAL;
    }

    if (snprintf(cmd, sizeof(cmd), "AT+CPBS=\"%s\"", storage) < 0)
    {
        DEBUG_PRINTF("ERROR: in creating string \"AT+CPBS=\"%s\"\"\r\n", storage);
        return EINVAL;
    }

    if (password != NULL)
    {
        if (strlen(password) > 8)
        {
            return EINVAL;
        }
        strcat(cmd, password);
    }

    // I think only "SM","ME","MT" are appropriate storage sources

    // +CPBS=<storage>[,<password>]
    ret |= rc7620_send_command(cmd, response, sizeof(response), TIMEOUT_30S);

    if (ret || !rc7620_check_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    return ret;
}

uint8_t rc7620_select_sim_phonebook_memory(void)
{
    return rc7620_select_phonebook_memory("SM", NULL);
}

uint8_t rc7620_query_phonebook_memory(int *used, int *total)
{
    char response[100];
    uint8_t ret = 0;
    int used_val, total_val;
    int matches = 0;

    ret |= rc7620_send_command("+CPBS?", response, sizeof(response), TIMEOUT_30S);

    if (ret || !rc7620_check_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    // +CPBS: <storage>[,<used>,<total>]
    matches = sscanf(response, "+CPBS: %d,%d",
                     &used_val, &total_val);

    if (matches != 2)
    {
        DEBUG_PRINTF("Bad Matches on Response: %s\r\n", response);
        return EBADMSG;
    }

    memcpy(used, &used_val, sizeof(int));
    memcpy(total, &total_val, sizeof(int));

    return ret;
}

uint8_t rc7620_get_phonebook_info(rc7620_phonebook__t *phonebook)
{
    char response[100];
    uint8_t ret = 0;
    int matches = 0;
    uint16_t index_min, index_max, nlength, tlength;

    ret |= rc7620_send_command("AT+CPBR=?", response, sizeof(response), TIMEOUT_30S) || !rc7620_check_ok(response);

    if (ret || !rc7620_check_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    matches = sscanf(response, "+CPBR: (%hd-%hd),%hd,%hd",
                     &index_min, &index_max, &nlength, &tlength);

    if (matches != 4)
    {
        DEBUG_PRINTF("Bad Matches on Response: %s\r\n", response);
        return EBADMSG;
    }

    phonebook->nlength = nlength;
    phonebook->tlength = tlength;
    phonebook->index_min = index_min;
    phonebook->index_max = index_max;

    return ret;
}

uint8_t rc7620_get_phonebook_entry_range(uint16_t index1, uint16_t index2, rc7620_phonebook__t *phonebook)
{
    char response[100];
    char cmd[24];
    uint8_t ret = 0;
    int matches = 0;

    // TODO: verfy index vals

    //+CPBR=<index1>[,<index2>]
    if (snprintf(cmd, sizeof(cmd), "AT+CPBR=%hd,%hd", index1, index2) < 0)
    {
        DEBUG_PRINTF("ERROR: in creating string \"AT+CPBR=%hd,%hd\"\r\n", index1, index2);
        return EINVAL;
    }

    ret |= rc7620_send_command(cmd, response, sizeof(response), TIMEOUT_30S);

    if (ret || !rc7620_check_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    // not sure best way to parse
    // TODO: figure out parsing method
    
    return ENOSYS;
}

uint8_t rc7620_cache_phonebook(void)
{
    return ENOSYS;
}

uint8_t rc7620_write_phonebook_entry_index(uint16_t index, rc7620_phonebook_entry_t *entry)
{

    char response[100];
    char cmd[100];
    uint8_t ret = 0;
    int matches = 0;

    // TODO: verfy index vals

    //+CPBW=[<index>][,<number>[,<type>[,<text>]]]
    if (snprintf(cmd, sizeof(cmd), "AT+CPBW=%hd,%s,%hd,%s",
                 entry->index, entry->number, entry->type, entry->text) < 0)
    {
        DEBUG_PRINTF("ERROR: in creating string \"AT+CPBW=%s,%hd,%s\"\r\n",
                     entry->index, entry->number, entry->type, entry->text);
        return EINVAL;
    }

    ret |= rc7620_send_command(cmd, response, sizeof(response), TIMEOUT_30S);

    if (ret || !rc7620_check_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    //TODO figure out if we can get out a response worth caring
    DEBUG_PRINTF("Response: %s\r\n", response);

    return ret;
}

uint8_t rc7620_delete_phonebook_entry(uint16_t index)
{

    char response[100];
    char cmd[100];
    uint8_t ret = 0;
    int matches = 0;

    // TODO: verfy index vals

    if (snprintf(cmd, sizeof(cmd), "AT+CPBW=%hd", index) < 0)
    {
        DEBUG_PRINTF("ERROR: in creating string \"AT+CPBW=%hd\"\r\n", index);
        return EINVAL;
    }

    ret |= rc7620_send_command(cmd, response, sizeof(response), TIMEOUT_30S);

    if (ret || !rc7620_check_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    return ret;
}

uint8_t rc7620_write_phonebook_entry_first(rc7620_phonebook_entry_t *entry)
{

    char response[100];
    char cmd[100];
    uint8_t ret = 0;
    int matches = 0;

    // TODO: verfy index vals

    //+CPBW=[,<number>[,<type>[,<text>]]]
    if (snprintf(cmd, sizeof(cmd), "AT+CPBW=,%s,%hd,%s",
                 entry->number, entry->type, entry->text) < 0)
    {
        DEBUG_PRINTF("ERROR: in creating string \"AT+CPBW=,%s,%hd,%s\"\r\n",
                     entry->number, entry->type, entry->text);
        return EINVAL;
    }

    ret |= rc7620_send_command(cmd, response, sizeof(response), TIMEOUT_30S);

    if (ret || !rc7620_check_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    //TODO figure out if we can get out an index
    DEBUG_PRINTF("Response: %s\r\n", response);

    return ret;
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

    // Set ring indicator  4 | 8 | 16 = 28 -> Incomming Call, data call & text
    DEBUG_PRINTF("Sending: AT+WWAKESET=28\r\n");
    rc7620_send_command("AT+WWAKESET=28", response, sizeof(response), default_timeout);
    DEBUG_PRINTF("Response: %s\r\n", response);
    HAL_Delay(100);

    // TEMP: find out what are the supported charsets
    DEBUG_PRINTF("Sending: AT+CSCS=?\r\n");
    rc7620_send_command("AT+CSCS=?", response, sizeof(response), default_timeout);
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
    DEBUG_PRINTF("Requesting SMS display for index %d: %s\r\n", index, command_buffer);

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
