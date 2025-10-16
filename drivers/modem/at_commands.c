#include <stdbool.h>
#include <stdlib.h>
#include "at_commands.h"
#include "stm32h7xx_hal.h"

uint8_t at_set_function_mode(enum FunctionModes mode)
{
    char response[32];
    char cmd[10];
    uint8_t ret = 0;

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

    ret |= modem_send_command(cmd, response, sizeof(response), TIMEOUT_5S);

    if (ret || !modem_check_response_ok(response)) // not sure this will actually respond with OK but ig we can see
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    return ret;
}

uint8_t at_set_auto_timezone(bool set_atz)
{
    char response[32];
    char cmd[10];
    uint8_t ret = 0;

    if (snprintf(cmd, sizeof(cmd), "AT+CTZU=%d", (uint8_t)set_atz) < 0)
    {
        DEBUG_PRINTF("ERROR: in creating string \"AT+CTZU=%d\"\r\n", (uint8_t)set_atz);
        return EINVAL;
    }

    ret |= modem_send_command(cmd, response, sizeof(response), TIMEOUT_5S);

    if (ret || !modem_check_response_ok(response)) // not sure this will actually respond with OK but ig we can see
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    return ret;
}

uint8_t at_get_clock(RTC_DateTypeDef *date, RTC_TimeTypeDef *time)
{
    char response[100];
    uint8_t ret = 0;
    int8_t utcoffset;

    RTC_DateTypeDef datestructure;
    RTC_TimeTypeDef timestructure;

    ret |= modem_send_command("AT+CCLK?", response, sizeof(response), TIMEOUT_2S);

    if (!modem_check_response_ok(response))
    {
        return 52;
    }
    if (ret) // not sure this will actually respond with OK but ig we can see
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    // response should be in form \r\n+CCLK: "yy/MM/dd,hh:mm:ss"
    if (strlen(response) < 26)
    {
        DEBUG_PRINTF("Response:\"%s\" Too short\r\n", response);
        return EBADMSG;
    }

    const char *p = response;
    while (*p && *p != '"')
    {
        p++;
    }
    if (*p == '"')
        p++;

    // Example format: 80/01/06,02:28:31
    datestructure.Year = atoi(p);         // YY
    datestructure.Month = atoi(p + 3);    // MM
    datestructure.Date = atoi(p + 6);     // DD
    timestructure.Hours = atoi(p + 9);    // hh
    timestructure.Minutes = atoi(p + 12); // mm
    timestructure.Seconds = atoi(p + 15); // ss

    memcpy(date, &datestructure, sizeof(RTC_DateTypeDef));
    memcpy(time, &timestructure, sizeof(RTC_TimeTypeDef));

    return ret;
}

uint8_t at_get_signal_strength(int16_t *rssi, uint8_t *ber)
{
    uint8_t ret = 0;
    char response[100];
    int matches = 0;
    uint8_t rssi_val, ber_val;
    int16_t rssi_val_db = 0; // unknown val

    ret |= modem_send_command("AT+CSQ", response, sizeof(response), TIMEOUT_2S);

    if (ret || !modem_check_response_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    // response in form +CSQ: <rssi>,<ber>
    matches = sscanf(response, "AT+CSQ: %hhd,%hhd",
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

uint8_t at_select_char_set(char *chset)
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
        DEBUG_PRINTF("ERROR: in creating string \"AT+CSCS=\"%s\"\"\r\n", chset);
        return EINVAL;
    }

    ret |= modem_send_command(cmd, response, sizeof(response), TIMEOUT_2S);

    if (ret || !modem_check_response_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    return ret;
}

uint8_t at_select_phonebook_memory(char storage[2], char *password)
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
    ret |= modem_send_command(cmd, response, sizeof(response), TIMEOUT_5S);

    if (ret || !modem_check_response_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    return ret;
}

uint8_t at_query_phonebook_memory(int *used, int *total)
{
    char response[100];
    uint8_t ret = 0;
    int used_val, total_val;
    int matches = 0;

    ret |= modem_send_command("+CPBS?", response, sizeof(response), TIMEOUT_5S);

    if (ret || !modem_check_response_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    // +CPBS: <storage>[,<used>,<total>]
    matches = sscanf(response, "AT+CPBS: %d,%d",
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

uint8_t at_get_phonebook_info(phonebook_t *phonebook)
{
    char response[100];
    uint8_t ret = 0;
    int matches = 0;
    uint16_t index_min, index_max, nlength, tlength, glength, slength, elength;

    ret |= modem_send_command("AT+CPBR=?", response, sizeof(response), TIMEOUT_5S) || !modem_check_response_ok(response);

    if (ret || !modem_check_response_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    matches = sscanf(response, "AT+CPBR: (%hd-%hd),%hd,%hd,%hd,%hd,%hd",
                     &index_min, &index_max, &nlength, &tlength, &glength, &slength, &elength);

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

uint8_t at_get_phonebook_entry_range(uint16_t index1, uint16_t index2, phonebook_t *phonebook)
{
    char response[100];
    char cmd[24];
    uint8_t ret = 0;
    // int matches = 0;

    // TODO: verfy index vals

    //+CPBR=<index1>[,<index2>]
    if (snprintf(cmd, sizeof(cmd), "AT+CPBR=%hd,%hd", index1, index2) < 0)
    {
        DEBUG_PRINTF("ERROR: in creating string \"AT+CPBR=%hd,%hd\"\r\n", index1, index2);
        return EINVAL;
    }

    ret |= modem_send_command(cmd, response, sizeof(response), TIMEOUT_5S);

    if (ret || !modem_check_response_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    // not sure best way to parse
    // TODO: figure out parsing method
    DEBUG_PRINTF("NOT IMPLIMENTED!");
    return ENOSYS;
}

uint8_t at_write_phonebook_entry_index(uint16_t index, phonebook_entry_t *entry)
{

    char response[100];
    char cmd[100];
    uint8_t ret = 0;
    // int matches = 0;

    // TODO: verfy index vals

    //+CPBW=[<index>][,<number>[,<type>[,<text>]]]
    if (snprintf(cmd, sizeof(cmd), "AT+CPBW=%hd,%s,%hd,%s",
                 entry->index, entry->number, entry->type, entry->text) < 0)
    {
        DEBUG_PRINTF("ERROR: in creating string \"AT+CPBW=%hd,%s,%hd,%s\"\r\n",
                     entry->index, entry->number, entry->type, entry->text);
        return EINVAL;
    }

    ret |= modem_send_command(cmd, response, sizeof(response), TIMEOUT_5S);

    if (ret || !modem_check_response_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    // TODO figure out if we can get out a response worth caring
    DEBUG_PRINTF("Response: %s\r\n", response);

    return ret;
}

uint8_t at_delete_phonebook_entry(uint16_t index)
{

    char response[100];
    char cmd[100];
    uint8_t ret = 0;
    // int matches = 0;

    // TODO: verfy index vals

    if (snprintf(cmd, sizeof(cmd), "AT+CPBW=%hd", index) < 0)
    {
        DEBUG_PRINTF("ERROR: in creating string \"AT+CPBW=%hd\"\r\n", index);
        return EINVAL;
    }

    ret |= modem_send_command(cmd, response, sizeof(response), TIMEOUT_5S);

    if (ret || !modem_check_response_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    return ret;
}

uint8_t at_write_phonebook_entry_first(phonebook_entry_t *entry)
{

    char response[100];
    char cmd[100];
    uint8_t ret = 0;
    // int matches = 0;

    // TODO: verfy index vals

    //+CPBW=[,<number>[,<type>[,<text>]]]
    if (snprintf(cmd, sizeof(cmd), "AT+CPBW=,%s,%hd,%s",
                 entry->number, entry->type, entry->text) < 0)
    {
        DEBUG_PRINTF("ERROR: in creating string \"AT+CPBW=,%s,%hd,%s\"\r\n",
                     entry->number, entry->type, entry->text);
        return EINVAL;
    }

    ret |= modem_send_command(cmd, response, sizeof(response), TIMEOUT_5S);

    if (ret || !modem_check_response_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    // TODO figure out if we can get out an index
    DEBUG_PRINTF("Response: %s\r\n", response);

    return ret;
}

uint8_t at_check_cpin(void)
{
    char response[32];
    uint8_t ret = 0;

    ret |= modem_send_command("AT+CPIN?", response, sizeof(response), TIMEOUT_5S);

    // Currently not a functional method, does not need to return data
    DEBUG_PRINTF("Response: %s\r\n", response);

    if (!strstr(response, "READY"))
    {
        return EBUSY;
    }

    return ret;
}

uint8_t at_get_sim_info(siminfo_t *sim_info)
{

    // commands used in the freertos implimentation

    //     "AT+CIMI",
    //     sim_info->imsi,

    //     "AT+CRSM=176,28514,0,0,0",
    //     &sim_info->plmn,
    DEBUG_PRINTF("NOT IMPLIMENTED!");
    return ENOSYS;
}

uint8_t at_get_modem_info(modeminfo_t *modem_info)
{

    // commands used in the freertos implimentation
    // atReqGetFirmwareVersion.pAtCmd = "AT+CGMR";
    // modem_info->firmwareVersion;

    // atReqGetImei.pAtCmd = "AT+CGSN";
    // modem_info->imei;

    // atReqGetModelId.pAtCmd = "AT+CGMM";
    // modem_info->modelId;

    // atReqGetManufactureId.pAtCmd = "AT+CGMI";
    // modem_info->manufactureId;
    DEBUG_PRINTF("NOT IMPLIMENTED!");
    return ENOSYS;
}

uint8_t at_check_net_reg(void)
{
    char response[32];
    uint8_t ret = 0;

    ret |= modem_send_command("AT+CREG?", response, sizeof(response), TIMEOUT_5S);

    // Currently not a functional method, does not need to return data
    DEBUG_PRINTF("Response: %s\r\n", response);

    return ret;
}

uint8_t at_check_eps_net_reg(void)
{
    char response[32];
    uint8_t ret = 0;

    ret |= modem_send_command("AT+CEREG?", response, sizeof(response), TIMEOUT_5S);

    // Currently not a functional method, does not need to return data
    DEBUG_PRINTF("Response: %s\r\n", response);

    return ret;
}

uint8_t at_set_message_format(enum TextModes mode)
{
    char response[32];
    char cmd[32];
    uint8_t ret = 0;

    // Verify Dialable correct?

    if (snprintf(cmd, sizeof(cmd), "AT+CMGF=%d", mode) < 0)
    {
        DEBUG_PRINTF("ERROR: in creating string \"AT+CMGF%d\"\r\n", mode);
        return EINVAL;
    }

    ret |= modem_send_command(cmd, response, sizeof(response), TIMEOUT_5S);

    if (ret || !modem_check_response_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    return ret;
}

uint8_t at_get_sms_textmode(int index, char *sms_buff, int bufflen)
{
    char command_buffer[32];
    char response[512]; // Internal buffer
    uint8_t ret = 0;

    memset(response, 0, sizeof(response));

    snprintf(command_buffer, sizeof(command_buffer), "AT+CMGR=%d", index);
    DEBUG_PRINTF("Requesting SMS display for index %d: %s\r\n", index, command_buffer);

    ret |= modem_send_command(command_buffer, response, sizeof(response), TIMEOUT_5S);

    if (ret || !modem_check_response_ok(response))
    {
        DEBUG_PRINTF("Failed to send AT+CMGR command for index %d.\r\n", index);
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    // response in form +CMGR: <status>,<sender_address>,[<alpha>],<timestamp>[,<optional_fields>]<CR><LF>
    // <message_body><CR><LF>
    // <CR><LF>
    // OK<CR><LF>
    if (strstr(response, "+CMGR:") != NULL)
    {
        DEBUG_PRINTF("--- SMS Index %d ---\r\n%s\r\n--- End SMS ---\r\n", index, response);
        // TODO: extract text info properly into a struct
    }
    else
    {
        ret = EBADMSG;
        DEBUG_PRINTF("Failed to display SMS content at index %d (Invalid response/Timeout):\r\n%s\r\n", index, response);
    }

    return ret;
}

uint8_t at_send_sms_textmode(const char *sms_address, const char *sms_message)
{
    char command_buffer[160];
    char response[128];
    uint8_t ret = 0;

    int cmd_len = snprintf(command_buffer, sizeof(command_buffer), "AT+CMGS=\"%s\"\r", sms_address);
    if (cmd_len < 0 || cmd_len >= sizeof(command_buffer))
    {
        DEBUG_PRINTF("Failed to format AT+CMGS command (number too long?).\r\n");
        return E2BIG;
    }

    DEBUG_PRINTF("Sending SMS command: %s", command_buffer);
    ret |= modem_write_command(command_buffer);
    if (ret)
    {
        DEBUG_PRINTF("Failed to write AT+CMGS command.\r\n");
        return ret;
    }

    HAL_Delay(100);
    memset(response, 0, sizeof(response));
    ret |= modem_read_response((uint8_t *)response, sizeof(response), TIMEOUT_1S);
    if (ret)
    {
        DEBUG_PRINTF("Did not receive prompt > within timeout.\r\n");
        return ret;
    }

    char *prompt_start = response;
    while (*prompt_start == '\r' || *prompt_start == '\n' || *prompt_start == ' ')
    {
        prompt_start++;
    }

    if (strncmp(prompt_start, "> ", 2) != 0)
    {
        DEBUG_PRINTF("Did not receive correct prompt >. Received:\r\n%s\r\n", response);
        return EBADMSG;
    }
    DEBUG_PRINTF("Received prompt >\r\n");

    DEBUG_PRINTF("Sending message body: %s\r\n", sms_message);
    ret |= modem_write_command(sms_message);
    if (ret)
    {
        DEBUG_PRINTF("Failed to write SMS message body.\r\n");
        return ret;
    }

    DEBUG_PRINTF("Sending Ctrl+Z (0x1A)\r\n");
    char ctrl_z[2] = {0x1A, 0x00};
    ret |= modem_write_command(ctrl_z);
    if (ret)
    {
        DEBUG_PRINTF("Failed to write Ctrl+Z.\r\n");
        return ret;
    }

    memset(response, 0, sizeof(response));
    ret |= modem_read_response((uint8_t *)response, sizeof(response), TIMEOUT_5S);
    if (ret)
    {
        DEBUG_PRINTF("Did not receive final response within timeout.\r\n");
        return ret;
    }

    DEBUG_PRINTF("Final response:\r\n%s\r\n", response);

    if (!(strstr(response, "+CMGS") != NULL && modem_check_response_ok(response)))
    {
        DEBUG_PRINTF("SMS send final response did not contain +CMGS and OK.\r\n");
        return EBADMSG;
    }

    DEBUG_PRINTF("SMS sent successfully.\r\n");
    return ret;
}

uint8_t at_call_status(call_status_t **status, int max_items)
{
    char response[512], *splt_ptr;
    uint8_t ret = 0;
    int matches, ccidx, dir, stat, mode, mpty;
    char number[MAXNUMBERSTR];

    memset(number, 0, MAXNUMBERSTR);

    // No defined timout
    ret |= modem_send_command("AT+CLCC", response, sizeof(response), TIMEOUT_2S);
    if (ret || !modem_check_response_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    // Confirm correct split
    splt_ptr = strtok(response, "\r\n");

    for (int i = 0; i < max_items && splt_ptr != NULL; i++)
    {

        // +CLCC: <ccid1>,<dir>,<stat>,<mode>,<mpty>, [number???]
        matches = sscanf(splt_ptr, "+CLCC: %d,%d,%d,%d,%d,%15s",
                         &ccidx, &dir, &stat, &mode, &mpty, number);

        if (matches != 6)
        {
            DEBUG_PRINTF("Bad Matches on Response: %s\r\n", response);
            return EBADMSG;
        }

        if (dir > DIR_MT)
        {
            DEBUG_PRINTF("Bad Dir Value %d\r\n", dir);
            return EBADMSG;
        }

        if (mode > MODE_UNKNOWN)
        {
            DEBUG_PRINTF("Bad mode Value %d\r\n", mode);
            return EBADMSG;
        }

        if (mpty > MPTY_YES)
        {
            DEBUG_PRINTF("Bad moty Value %d\r\n", mpty);
            return EBADMSG;
        }

        call_status_t *curr_status = status[i];
        curr_status->ccidx = ccidx;
        curr_status->dir = (enum CallDir)dir;
        curr_status->mode = (enum CallMode)mode;
        curr_status->mpty = (enum CallMPTY)mpty;
        memcpy(curr_status->number, number, 16);

        splt_ptr = strtok(NULL, "\r\n");
    }

    return ret;
}

uint8_t at_call_dial(char *dial_string, enum ATV0ResultCodes *result_code)
{
    char response[32];
    char cmd[32];
    uint8_t ret = 0;

    // Verify Dialable correct?

    if (snprintf(cmd, sizeof(cmd), "ATD%s;", dial_string) < 0)
    {
        DEBUG_PRINTF("ERROR: in creating string \"ATD%s\"\r\n", dial_string);
        return EINVAL;
    }

    ret |= modem_send_command(cmd, response, sizeof(response), TIMEOUT_2S);

    if (ret)
    {
        DEBUG_PRINTF("ERROR IN SEND");
        return EBADMSG;
    }

    if (strstr(response, "CONNECT") != NULL)
    {
        DEBUG_PRINTF("CONNECTED");
        *result_code = ATV0_CONNECT;
        return 0;
    }
    else if (strstr(response, "NO CARRIER") != NULL)
    {
        DEBUG_PRINTF("NO CARRIER");
        *result_code = ATV0_NO_CARRIER;
        return 0;
    }
    else if (strstr(response, "ERROR") != NULL)
    {
        DEBUG_PRINTF("ERROR");
        *result_code = ATV0_ERROR;
        return 0;
    }
    else if (strstr(response, "BUSY") != NULL)
    {
        DEBUG_PRINTF("BUSY");
        *result_code = ATV0_BUSY;
        return 0;
    }
    else if (strstr(response, "NO ANSWER") != NULL)
    {
        DEBUG_PRINTF("NO ANSWER");
        *result_code = ATV0_NO_ANSWER;
        return 0;
    }
    else if (strstr(response, "NO DAILTONE") != NULL)
    {
        DEBUG_PRINTF("NO DAILTONE");
        *result_code = ATV0_NO_DAILTONE;
        return 0;
    }
    else if (strstr(response, "OK") != NULL)
    {
        DEBUG_PRINTF("OK");
        *result_code = ATV0_OK;
        return 0;
    }
    else
    {
        DEBUG_PRINTF("CRITICAL ERROR: COULD NOT MATCH RETURN ON DIAL");
        return EBADMSG;
    }

    return ret;
}

uint8_t at_call_answer(enum ATV0ResultCodes *result_code)
{
    char response[32];
    uint8_t ret = 0;

    ret |= modem_send_command("ATA", response, sizeof(response), TIMEOUT_2S);

    if (ret)
    {
        DEBUG_PRINTF("ERROR IN SEND");
        return EBADMSG;
    }

    if (strstr(response, "CONNECT") != NULL)
    {
        DEBUG_PRINTF("CONNECTED");
        *result_code = ATV0_CONNECT;
        return 0;
    }
    else if (strstr(response, "NO CARRIER") != NULL)
    {
        DEBUG_PRINTF("NO CARRIER");
        *result_code = ATV0_NO_CARRIER;
        return 0;
    }
    else if (strstr(response, "ERROR") != NULL)
    {
        DEBUG_PRINTF("ERROR");
        *result_code = ERROR;
        return 0;
    }
    else if (strstr(response, "OK") != NULL)
    {
        DEBUG_PRINTF("OK");
        *result_code = ATV0_OK;
        return 0;
    }
    else
    {
        DEBUG_PRINTF("CRITICAL ERROR: COULD NOT MATCH RETURN ON DIAL");
        return EBADMSG;
    }
    return ret;
}

uint8_t at_call_hook(void)
{
    char response[32];
    uint8_t ret = 0;

    ret |= modem_send_command("ATH0", response, sizeof(response), TIMEOUT_2S);

    if (ret || !modem_check_response_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    return ret;
}

uint8_t at_set_echo(bool echo)
{
    char response[32];
    char cmd[32];
    uint8_t ret = 0;

    if (snprintf(cmd, sizeof(cmd), "ATE%d", (uint8_t)echo) < 0)
    {
        DEBUG_PRINTF("ERROR: in creating string \"ATE%d\"\r\n", (uint8_t)echo);
        return EINVAL;
    }

    ret |= modem_send_command(cmd, response, sizeof(response), TIMEOUT_2S);

    if (ret || !modem_check_response_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return EBADMSG;
    }

    return ret;
}