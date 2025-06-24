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

    // test AT startup
    DEBUG_PRINTF("Sending: AT\r\n");
    ret |= modem_send_command("AT", response, sizeof(response), default_timeout);
    if (ret || !modem_check_response_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return ret;
    }
    DEBUG_PRINTF("Response: %s\r\n", response);
    HAL_Delay(100);

    // disable echo
    DEBUG_PRINTF("Sending: ATE0\r\n");
    modem_send_command("ATE0", response, sizeof(response), default_timeout);
    DEBUG_PRINTF("Response: %s\r\n", response);
    HAL_Delay(100);

    // disable GPS
    DEBUG_PRINTF("Sending: AT!CUSTOM=\"GPSENABLE\",0\r\n");
    modem_send_command("AT!CUSTOM=\"GPSENABLE\",0", response, sizeof(response), default_timeout);
    DEBUG_PRINTF("Response: %s\r\n", response);
    HAL_Delay(100);

    // check SIM status and/or needs pin
    DEBUG_PRINTF("Sending: AT+CPIN?\r\n");
    ret |= modem_send_command("AT+CPIN?", response, sizeof(response), default_timeout);
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
    modem_send_command("AT+GSN", response, sizeof(response), default_timeout);
    DEBUG_PRINTF("Response: %s\r\n", response);
    HAL_Delay(100);

    // check network registration status
    DEBUG_PRINTF("Sending: AT+CREG?\r\n");
    modem_send_command("AT+CREG?", response, sizeof(response), default_timeout);
    DEBUG_PRINTF("Response: %s\r\n", response);
    HAL_Delay(100);

    DEBUG_PRINTF("Sending: AT+CEREG?\r\n");
    modem_send_command("AT+CEREG?", response, sizeof(response), default_timeout);
    DEBUG_PRINTF("Response: %s\r\n", response);
    HAL_Delay(100);

    // set phone functionality 1 (full functionality, high power draw)
    DEBUG_PRINTF("Sending: AT+CFUN=1\r\n");
    ret |= modem_send_command("AT+CFUN=1", response, sizeof(response), 5000);
    if (ret || !modem_check_response_ok(response))
    {
        DEBUG_PRINTF("Response: %s\r\n", response);
        return 0;
    }
    DEBUG_PRINTF("Response: %s\r\n", response);
    HAL_Delay(100);

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
    DEBUG_PRINTF("Setting SMS text mode: AT+CMGF=1\r\n");
    ret |= modem_send_command("AT+CMGF=1", response, sizeof(response), TIMEOUT_1S);
    if (ret || !modem_check_response_ok(response))
    {
        DEBUG_PRINTF("Failed to set SMS mode to text. Response:\r\n%s\r\n", response);
        return EBADMSG;
    }
    DEBUG_PRINTF("SMS mode set. Response:\r\n%s\r\n", response);
    HAL_Delay(100);

    return ret; // Initialization successful
}