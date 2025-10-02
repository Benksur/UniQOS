#ifndef MULTITAP_H
#define MULTITAP_H

#include <stdint.h>
#include <stdbool.h>
#include "input.h"

// No timing constants needed - purely key-based

// Nokia-style key mappings
typedef struct
{
    char characters[5]; // Up to 4 characters + null terminator
    uint8_t count;      // Number of characters for this key
} multitap_key_t;

// Multi-tap state
typedef struct
{
    uint32_t last_key;     // Last pressed key (INPUT_KEYPAD_*)
    uint8_t current_index; // Current character index for the key
    bool is_active;        // Whether multi-tap is currently active
    char pending_char;     // Character to be inserted when # is pressed
} multitap_state_t;

// Function declarations
void multitap_init(void);
void multitap_reset(void);
bool multitap_handle_keypress(input_event_t key_event, char *output_char);
bool multitap_is_active(void);
char multitap_get_current_char(void);
bool multitap_confirm_character(char *output_char);

// Key mapping access
const multitap_key_t *multitap_get_key_mapping(input_event_t key_event);

#endif // MULTITAP_H
