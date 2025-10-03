#include "multitap.h"
#include <stdlib.h>

// Nokia-style key mappings
static const multitap_key_t key_mappings[] = {
    [INPUT_KEYPAD_0] = {.characters = " 0", .count = 2},    // Space and 0
    [INPUT_KEYPAD_1] = {.characters = ".,!?1", .count = 5}, // Punctuation and 1
    [INPUT_KEYPAD_2] = {.characters = "abc2", .count = 4},  // abc and 2
    [INPUT_KEYPAD_3] = {.characters = "def3", .count = 4},  // def and 3
    [INPUT_KEYPAD_4] = {.characters = "ghi4", .count = 4},  // ghi and 4
    [INPUT_KEYPAD_5] = {.characters = "jkl5", .count = 4},  // jkl and 5
    [INPUT_KEYPAD_6] = {.characters = "mno6", .count = 4},  // mno and 6
    [INPUT_KEYPAD_7] = {.characters = "pqrs7", .count = 5}, // pqrs and 7
    [INPUT_KEYPAD_8] = {.characters = "tuv8", .count = 4},  // tuv and 8
    [INPUT_KEYPAD_9] = {.characters = "wxyz9", .count = 5}, // wxyz and 9
    [INPUT_KEYPAD_STAR] = {.characters = "*", .count = 1},  // Star key
    [INPUT_KEYPAD_HASH] = {.characters = "#", .count = 1},  // Hash key
};

// Global multi-tap state
static multitap_state_t multitap_state = {0};

void multitap_init(void)
{
    multitap_reset();
}

void multitap_reset(void)
{
    multitap_state.last_key = INPUT_NONE;
    multitap_state.current_index = 0;
    multitap_state.is_active = false;
    multitap_state.pending_char = '\0';
}

bool multitap_handle_keypress(input_event_t key_event, char *output_char)
{
    if (output_char == NULL)
    {
        return false;
    }

    // Only handle keypad events (excluding # which is handled separately)
    if (key_event < INPUT_KEYPAD_0 || key_event > INPUT_KEYPAD_STAR)
    {
        return false;
    }

    const multitap_key_t *key_mapping = &key_mappings[key_event];

    // Check if this is the same key as the last press
    if (key_event == multitap_state.last_key && multitap_state.is_active)
    {
        // Same key pressed - cycle through characters
        multitap_state.current_index = (multitap_state.current_index + 1) % key_mapping->count;
        multitap_state.pending_char = key_mapping->characters[multitap_state.current_index];

        // Don't output character yet - wait for # confirmation
        return false;
    }

    // Different key pressed - output previous character if any
    if (multitap_state.is_active && multitap_state.pending_char != '\0')
    {
        *output_char = multitap_state.pending_char;
        multitap_reset();
        return true;
    }

    // Start new multi-tap sequence
    multitap_state.last_key = key_event;
    multitap_state.current_index = 0;
    multitap_state.is_active = true;
    multitap_state.pending_char = key_mapping->characters[0];

    // Don't output character yet - wait for # confirmation
    return false;
}

bool multitap_is_active(void)
{
    return multitap_state.is_active;
}

char multitap_get_current_char(void)
{
    if (!multitap_state.is_active)
    {
        return '\0';
    }
    return multitap_state.pending_char;
}

bool multitap_confirm_character(char *output_char)
{
    if (output_char == NULL || !multitap_state.is_active)
    {
        return false;
    }

    if (multitap_state.pending_char != '\0')
    {
        *output_char = multitap_state.pending_char;
        multitap_reset();
        return true;
    }

    return false;
}

const multitap_key_t *multitap_get_key_mapping(input_event_t key_event)
{
    if (key_event < INPUT_KEYPAD_0 || key_event > INPUT_KEYPAD_HASH)
    {
        return NULL;
    }
    return &key_mappings[key_event];
}
