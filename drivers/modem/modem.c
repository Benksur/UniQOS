#include "modem.h"
#include "stm32h7xx_hal.h"

enum FunctionModes curr_function_mode = MODE_MIN;

uint8_t modem_toggle_airplane_mode(void)
{
    uint8_t ret;
    if (curr_function_mode == MODE_AIRPLANE)
    {
        ret = modem_set_function_mode(MODE_FULL);
        if (!ret) {
            curr_function_mode = MODE_FULL;
        }
    }
    else
    {
        ret = modem_set_function_mode(MODE_AIRPLANE);
        if (!ret) {
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

    // test AT startup
    DEBUG_PRINTF("Sending: AT\r\n");
    ret |= modem_send_command("AT", response, sizeof(response), TIMEOUT_2S);
    if (ret || !modem_check_response_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return ret;
    }
    DEBUG_PRINTF("Response: %s\r\n", response);
    HAL_Delay(100);

    // disable echo
    ret |= at_set_echo(false);
    if (ret) {
        DEBUG_PRINTF("FATAL ERROR ON INIT: ECHO");
        return ret;
    }
    HAL_Delay(100);

    // disable GPS
    ret |= at_custom("GPSENABLE", 0);
    if (ret) {
        DEBUG_PRINTF("FATAL ERROR ON INIT: GPSENABLE");
        return ret;
    }
    HAL_Delay(100);

    // check SIM status and/or needs pin
    ret |= at_check_cpin();
    if (ret) {
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
    HAL_Delay(100);

    // check network registration status (Only debug out)
    at_check_net_reg();
    HAL_Delay(100);

    at_check_eps_net_reg();
    HAL_Delay(100);

    // set phone functionality 1 (full functionality, high power draw)
    ret |= at_set_function_mode(MODE_FULL);
    if (ret) {
        DEBUG_PRINTF("FATAL ERROR ON INIT: FUNCTION MODE");
        return ret;
    }
    HAL_Delay(100);

    ret |= at_set_auto_timezone(true);
    if (ret) {
        DEBUG_PRINTF("FATAL ERROR ON INIT: AUTO TIMEZONE");
        return ret;
    }

    // Set APN to telstra IP
    DEBUG_PRINTF("Sending: AT+CGDCONT=1,\"IP\",\"telstra.internet\"\r\n");
    modem_send_command("AT+CGDCONT=1,\"IP\",\"telstra.internet\"", response, sizeof(response), default_timeout);
    DEBUG_PRINTF("Response: %s\r\n", response);
    HAL_Delay(100);

    // notify on new sms
    DEBUG_PRINTF("Sending: AT+CNMI=1,1,0,0,0\r\n");
    modem_send_command("AT+CNMI=1,1,0,0,0", response, sizeof(response), default_timeout);
    DEBUG_PRINTF("Response: %s\r\n", response);
    HAL_Delay(100);

    // Set ring indicator  4 | 8 | 16 = 28 -> Incomming Call, data call & text
    DEBUG_PRINTF("Sending: AT+WWAKESET=28\r\n");
    modem_send_command("AT+WWAKESET=28", response, sizeof(response), default_timeout);
    DEBUG_PRINTF("Response: %s\r\n", response);
    HAL_Delay(100);

    // Setting message format
    at_set_message_format(TEXTMODE_TEXT);
    if (ret) {
        DEBUG_PRINTF("FATAL ERROR ON INIT: TEXTMODE");
        return ret;
    }
    HAL_Delay(100);

    return ret; // Initialization successful
}