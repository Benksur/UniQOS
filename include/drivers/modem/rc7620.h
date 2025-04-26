#ifndef RC7620_H_
#define RC7620_H_

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "errornum.h"
#include "stm32_config.h"

enum FunctionModes
{
    MODE_MIN = 0,      // minimum functionality, low power draw
    MODE_FULL = 1,     // full functionality, high power draw
    MODE_AIRPLANE = 4, // Airplane mode, low power draw
    MODE_TEST = 5,     // Factory Test Mode
    MODE_RST = 6,      // Reset UE
    MODE_OFFLINE = 7,  // Offline mode
};

// Standard Command Timeout Values
#define TIMEOUT_2S 2000
#define TIMEOUT_5S 5000
#define TIMEOUT_30S 30000
#define TIMEOUT_60S 60000
#define TIMEOUT_120S 120000

// TODO: Determine max string sizes to set a fixed size
typedef struct
{
    uint16_t index;   // index in the source phone book
    char *number;     // string type phone number of format `type`
    uint16_t type;    // default 145 when dialling string includes international access code character "+", otherwise 129
    char *text;       // string type field of maximum length <tlength>; character set as specified by command select TE character set +CSCS
} rc7620_phonebook_entry_t;

typedef struct
{
    uint16_t index_min; // min index value
    uint16_t index_max; // max index value
    uint16_t nlength;   // maximum length of rc7620_phonebook_entry_t field `number`
    uint16_t tlength;   // maximum length of rc7620_phonebook_entry_t field `text`
    rc7620_phonebook_entry_t *entries;
} rc7620_phonebook__t;

uint8_t rc7620_write_command(const char *command);
uint8_t rc7620_read_response(uint8_t *buffer, uint16_t max_len, uint32_t timeout);
uint8_t rc7620_send_command(const char *command, char *response, uint16_t response_size, uint32_t read_timeout);
uint8_t rc7620_check_ok(const char *response);
uint8_t rc7620_init(void);
uint8_t rc7620_select_sim_phonebook_memory(void);
uint8_t rc7620_query_phonebook_memory(int *used, int *total);

#endif
