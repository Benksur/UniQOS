#ifndef MODEM_H_
#define MODEM_H_

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "errornum.h"
#include "stm32_config.h"
#include "at_commands.h"

uint8_t modem_init(void);
uint8_t modem_sleep(void);
uint8_t modem_airplane_mode_on(void);
uint8_t modem_airplane_mode_off(void);
uint8_t modem_send_sms(const char *sms_address, const char *sms_message);
uint8_t modem_dial(const char *dial_string);
uint8_t modem_get_signal_strength(int16_t *rssi, uint8_t *ber);

#endif