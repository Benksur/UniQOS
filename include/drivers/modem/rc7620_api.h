#ifndef RC7620_API_H_
#define RC7620_API_H_

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "errornum.h"
#include "stm32_config.h"

// Change to actual password in production 
// DO NOT COMMIT WITH ACTUAL PASSWORD
#define RC7620_AT_PASSWORD "12345"

uint8_t modem_write_command(const char *command);

uint8_t modem_read_response(uint8_t *buffer, uint16_t max_len, uint32_t timeout);

uint8_t modem_send_command(const char *command, char *response, uint16_t response_size, uint32_t read_timeout);

uint8_t modem_check_response_ok(const char *response);

void modem_power_on(void);

void modem_power_off(void);

uint8_t at_custom(char* customization, uint8_t value);

#endif