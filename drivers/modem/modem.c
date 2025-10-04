#include "modem.h"
#include "stm32h7xx_hal.h"
#include <stdlib.h>

enum FunctionModes curr_function_mode = MODE_MIN;

uint8_t modem_toggle_airplane_mode(void)
{
    uint8_t ret;
    if (curr_function_mode == MODE_AIRPLANE)
    {
        ret = at_set_function_mode(MODE_FULL);
        if (!ret)
        {
            curr_function_mode = MODE_FULL;
        }
    }
    else
    {
        ret = at_set_function_mode(MODE_AIRPLANE);
        if (!ret)
        {
            curr_function_mode = MODE_AIRPLANE;
        }
    }

    return ret;
}

uint8_t modem_init(void)
{
    char response[128];
    const uint32_t default_timeout = TIMEOUT_2S;
    uint8_t ret = 0;
    modeminfo_t modem_info;

    modem_power_on();
    HAL_Delay(1000);

    // test AT startup
    while (1)
    {
        DEBUG_PRINTF("Sending: AT\r\n");
        ret |= modem_send_command("AT", response, sizeof(response), TIMEOUT_2S);
        if (ret || !modem_check_response_ok(response))
        {
            DEBUG_PRINTF("Response: %s\r\n", response);
        }
        else
        {
            break;
        }
        DEBUG_PRINTF("Response: %s\r\n", response);
        HAL_Delay(100);
    }

    // disable echo
    ret |= at_set_echo(false);
    if (ret)
    {
        DEBUG_PRINTF("FATAL ERROR ON INIT: ECHO");
        return ret;
    }
    HAL_Delay(100);

    // disable GPS
    ret |= at_custom("GPSENABLE", 0);
    if (ret)
    {
        DEBUG_PRINTF("FATAL ERROR ON INIT: GPSENABLE");
        return ret;
    }
    HAL_Delay(100);

    // check SIM status and/or needs pin
    ret |= at_check_cpin();
    if (ret)
    {
        DEBUG_PRINTF("FATAL ERROR ON INIT: SIM PASSWORD");
        return ret;
    }
    HAL_Delay(100);

    // get info - NOT IMPLIMENTED
    // ret |= at_get_modem_info(&modem_info);
    // if (ret) {
    //     DEBUG_PRINTF("FATAL ERROR ON INIT: MODEM INFO");
    //     return ret;
    // }
    // HAL_Delay(100);

    // check network registration status (Only debug out)
    // at_check_net_reg();
    // HAL_Delay(100);

    // at_check_eps_net_reg();
    // HAL_Delay(100);

    // set phone functionality 1 (full functionality, high power draw)
    // ret |= at_set_function_mode(MODE_FULL);
    // if (ret)
    // {
    //     DEBUG_PRINTF("FATAL ERROR ON INIT: FUNCTION MODE");
    //     return ret;
    // }
    // HAL_Delay(100);

    ret |= at_set_auto_timezone(true);
    if (ret)
    {
        DEBUG_PRINTF("FATAL ERROR ON INIT: AUTO TIMEZONE");
        return ret;
    }

    // Set APN to telstra IP
    // DEBUG_PRINTF("Sending: AT+CGDCONT=1,\"IP\",\"telstra.internet\"\r\n");
    // modem_send_command("AT+CGDCONT=1,\"IP\",\"telstra.internet\"", response, sizeof(response), default_timeout);
    // DEBUG_PRINTF("Response: %s\r\n", response);
    // HAL_Delay(100);

    // notify on new sms
    // DEBUG_PRINTF("Sending: AT+CNMI=1,1,0,0,0\r\n");
    // modem_send_command("AT+CNMI=1,1,0,0,0", response, sizeof(response), default_timeout);
    // DEBUG_PRINTF("Response: %s\r\n", response);
    // HAL_Delay(100);

    // Set ring indicator  4 | 8 | 16 = 28 -> Incomming Call, data call & text
    DEBUG_PRINTF("Sending: AT+WWAKESET=28\r\n");
    modem_send_command("AT+WWAKESET=28", response, sizeof(response), default_timeout);
    DEBUG_PRINTF("Response: %s\r\n", response);
    HAL_Delay(100);

    // Enable caller ID presentation (CLIP)
    DEBUG_PRINTF("Sending: AT+CLIP=1\r\n");
    modem_send_command("AT+CLIP=1", response, sizeof(response), default_timeout);
    DEBUG_PRINTF("Response: %s\r\n", response);
    HAL_Delay(100);

    // Enable I2S Audio interface
    // 5 is the default audio profile
    modem_send_command("AT!AVCFG=5,0,1", response, sizeof(response), default_timeout);
    HAL_Delay(100);

    // Setting message format
    at_set_message_format(TEXTMODE_TEXT);
    if (ret)
    {
        DEBUG_PRINTF("FATAL ERROR ON INIT: TEXTMODE");
        return ret;
    }
    HAL_Delay(100);

    return ret; // Initialization successful
}

uint8_t modem_sleep(void)
{
    return at_set_function_mode(MODE_OFFLINE);
}

uint8_t modem_airplane_mode_on(void)
{
    return at_set_function_mode(MODE_AIRPLANE);
}

uint8_t modem_airplane_mode_off(void)
{
    return at_set_function_mode(MODE_FULL);
}

uint8_t modem_send_sms(const char *sms_address, const char *sms_message)
{
    return at_send_sms_textmode(sms_address, sms_message);
}

uint8_t modem_dial(const char *dial_string)
{
    return at_call_dial(dial_string, NULL);
}

uint8_t modem_get_signal_strength(int16_t *rssi, uint8_t *ber)
{
    return at_get_signal_strength(rssi, ber);
}

ModemEventType modem_check_event(char *caller_id, size_t caller_id_size, uint8_t *sms_index)
{
    char buffer[128];
    uint16_t received_len = 0;

    HAL_StatusTypeDef status = HAL_UARTEx_ReceiveToIdle(&MODEM_UART_HANDLE, (uint8_t *)buffer, sizeof(buffer) - 1, &received_len, 100);

    if ((status == HAL_OK || status == HAL_TIMEOUT) && received_len > 0)
    {
        // Null-terminate at actual received length
        buffer[received_len] = '\0';

        // Check for incoming call (RING or +CLIP with caller ID)
        if (strstr(buffer, "RING") != NULL)
        {
            // Try to extract caller ID from +CLIP if present
            char *clip_start = strstr(buffer, "+CLIP:");
            if (clip_start != NULL && caller_id != NULL)
            {
                // Format: +CLIP: "<number>",<type>,...
                char *quote1 = strchr(clip_start, '"');
                if (quote1 != NULL)
                {
                    quote1++; // Move past first quote
                    char *quote2 = strchr(quote1, '"');
                    if (quote2 != NULL)
                    {
                        size_t len = quote2 - quote1;
                        if (len < caller_id_size)
                        {
                            strncpy(caller_id, quote1, len);
                            caller_id[len] = '\0';
                        }
                    }
                }
            }
            return MODEM_EVENT_INCOMING_CALL;
        }

        // Check for incoming SMS notification
        // Format: +CMTI: "<mem>",<index>
        // Example: +CMTI: "SM",5
        char *cmti_start = strstr(buffer, "+CMTI:");
        if (cmti_start != NULL)
        {
            if (sms_index != NULL)
            {
                // Find the comma separating memory type and index
                char *comma = strchr(cmti_start, ',');
                if (comma != NULL)
                {
                    // Parse the index number after the comma
                    *sms_index = (uint8_t)atoi(comma + 1);
                }
            }
            return MODEM_EVENT_INCOMING_SMS;
        }
    }

    return MODEM_EVENT_NONE;
}

uint8_t modem_read_sms(uint8_t index, char *sender, size_t sender_size,
                       char *message, size_t message_size)
{
    char response[512];
    char cmd[16];
    snprintf(cmd, sizeof(cmd), "AT+CMGR=%d", index);

    if (modem_send_command(cmd, response, sizeof(response), TIMEOUT_5S) != 0)
        return 1; // error

    // Parse sender number (second quoted field)
    // Format: +CMGR: "status","phone_number",,"timestamp"
    char *p = strstr(response, "+CMGR:");
    if (p)
    {
        // Skip first quoted field (status)
        char *q1 = strchr(p, '"');
        if (q1)
        {
            q1++; // Move past first quote
            char *q2 = strchr(q1, '"');
            if (q2)
            {
                // Now find second quoted field (phone number)
                char *q3 = strchr(q2 + 1, '"');
                if (q3)
                {
                    q3++; // Move past opening quote
                    char *q4 = strchr(q3, '"');
                    if (q4 && (q4 - q3) < sender_size)
                    {
                        strncpy(sender, q3, q4 - q3);
                        sender[q4 - q3] = '\0';
                    }
                }
            }
        }
    }

    // Find the actual message (after the header line)
    // The message is on the line(s) after the +CMGR header
    char *msg = strstr(response, "+CMGR:");
    if (msg)
    {
        // Find the end of the +CMGR header line
        msg = strstr(msg, "\r\n");
        if (msg)
        {
            msg += 2; // Skip \r\n to start of message

            // Find message end
            char *msg_end = strstr(msg, "\r\n\r\nOK");

            if (msg_end)
            {
                size_t len = msg_end - msg;
                if (len >= message_size)
                    len = message_size - 1;
                strncpy(message, msg, len);
                message[len] = '\0';
            }
        }
    }

    return 0;
}

uint8_t modem_hang_up(void)
{
    return at_call_hook();
}