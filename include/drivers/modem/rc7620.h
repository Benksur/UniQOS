#ifndef RC7620_H_
#define RC7620_H_

#include <stdint.h>
#include <string.h>
#include "board/stm32_config.h"

uint8_t rc7620_write_command(const char *command);
HAL_StatusTypeDef rc7620_read_response(uint8_t *buffer, uint16_t max_len, uint32_t timeout);
uint8_t rc7620_send_command(const char *command, char *response, uint16_t response_size, uint32_t read_timeout);
uint8_t rc7620_check_ok(const char *response);
uint8_t rc7620_init(void);

#endif
