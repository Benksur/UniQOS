/**
 * @file keypad.h
 * @brief Matrix keypad driver
 * @ingroup keypad_driver
 *
 * Driver for scanning and reading input from a matrix keypad.
 * Provides button state tracking and event generation.
 */

#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdbool.h>
#include <stdint.h>
#include "input.h"

/**
 * @ingroup keypad_driver
 * @brief Initialize the keypad driver
 *
 * Configures GPIO pins and initializes button state tracking.
 */
void keypad_init(void);

/**
 * @ingroup keypad_driver
 * @brief Update button states
 *
 * Scans the keypad matrix and updates button states.
 * Should be called periodically to detect button changes.
 */
void keypad_update_states(void);

/**
 * @ingroup keypad_driver
 * @brief Check if a button is currently pressed
 * @param button_index Button index to check
 * @return true if button is pressed, false otherwise
 */
bool keypad_is_button_pressed(uint8_t button_index);

/**
 * @ingroup keypad_driver
 * @brief Get button event for a specific button
 * @param button_index Button index to query
 * @return Input event structure with button state
 */
input_event_t keypad_get_button_event(uint8_t button_index);

/**
 * @ingroup keypad_driver
 * @brief Get total number of buttons
 * @return Number of buttons available on the keypad
 */
uint8_t keypad_get_button_count(void);

/**
 * @ingroup keypad_driver
 * @brief Read button event (legacy function)
 * @param out_event Pointer to receive button event
 * @return true if an event was available, false otherwise
 * @deprecated Use keypad_get_button_event() instead
 */
bool keypad_read_button(input_event_t *out_event);

#endif // KEYPAD_H