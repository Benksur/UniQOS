#ifndef MODEM_H_
#define MODEM_H_

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "errornum.h"
#include "stm32_config.h"
#include "at_commands.h"

typedef enum
{
    MODEM_EVENT_NONE = 0,
    MODEM_EVENT_INCOMING_CALL,
    MODEM_EVENT_INCOMING_SMS
} ModemEventType;

uint8_t modem_init(void);
uint8_t modem_sleep(void);
uint8_t modem_airplane_mode_on(void);
uint8_t modem_airplane_mode_off(void);
uint8_t modem_send_sms(const char *sms_address, const char *sms_message);
uint8_t modem_dial(const char *dial_string);
uint8_t modem_get_signal_strength(int16_t *rssi, uint8_t *ber);
uint8_t modem_get_clock(RTC_DateTypeDef *date, RTC_TimeTypeDef *time);
uint8_t modem_read_sms(uint8_t index, char *sender, size_t sender_size,
                       char *message, size_t message_size);
ModemEventType modem_check_event(char *caller_id, size_t caller_id_size, uint8_t *sms_index);
uint8_t modem_hang_up(void);
#endif