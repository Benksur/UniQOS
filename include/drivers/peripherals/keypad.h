#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdbool.h>
#include <stdint.h>
#include "input.h"

// Function declarations
void keypad_init(void);
void keypad_update_states(void);
bool keypad_is_button_pressed(uint8_t button_index);
input_event_t keypad_get_button_event(uint8_t button_index);
uint8_t keypad_get_button_count(void);

// Legacy function for backward compatibility
bool keypad_read_button(input_event_t* out_event);

#endif // KEYPAD_H 