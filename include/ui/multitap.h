/**
 * @file multitap.h
 * @brief Multitap text entry system
 * @ingroup ui_input
 *
 * Implements Nokia-style multitap text entry for numeric keypads.
 * Allows entering letters and symbols using repeated key presses.
 */

#ifndef MULTITAP_H
#define MULTITAP_H

#include <stdint.h>
#include <stdbool.h>
#include "input.h"

/**
 * @brief Multitap key mapping structure
 * @ingroup ui_input
 *
 * Maps a keypad button to its possible characters.
 */
typedef struct
{
    char characters[5]; /**< Up to 4 characters + null terminator */
    uint8_t count;      /**< Number of characters for this key */
} multitap_key_t;

/**
 * @brief Multitap state structure
 * @ingroup ui_input
 *
 * Tracks the current state of multitap text entry.
 */
typedef struct
{
    uint32_t last_key;     /**< Last pressed key (INPUT_KEYPAD_*) */
    uint8_t current_index; /**< Current character index for the key */
    bool is_active;        /**< Whether multi-tap is currently active */
    char pending_char;     /**< Character to be inserted when # is pressed */
} multitap_state_t;

/**
 * @ingroup ui_input
 * @brief Initialize the multitap system
 */
void multitap_init(void);

/**
 * @ingroup ui_input
 * @brief Reset multitap state
 *
 * Clears current input and resets to initial state.
 */
void multitap_reset(void);

/**
 * @ingroup ui_input
 * @brief Handle a keypress event
 * @param key_event Input event from keypad
 * @param output_char Pointer to receive output character (if any)
 * @return true if a character was output, false otherwise
 */
bool multitap_handle_keypress(input_event_t key_event, char *output_char);

/**
 * @ingroup ui_input
 * @brief Check if multitap is currently active
 * @return true if multitap is active, false otherwise
 */
bool multitap_is_active(void);

/**
 * @ingroup ui_input
 * @brief Get the current character being selected
 * @return Current character (0 if none)
 */
char multitap_get_current_char(void);

/**
 * @ingroup ui_input
 * @brief Confirm the current character
 * @param output_char Pointer to receive confirmed character
 * @return true if character was confirmed, false otherwise
 */
bool multitap_confirm_character(char *output_char);

/**
 * @ingroup ui_input
 * @brief Get key mapping for a specific key
 * @param key_event Input event to get mapping for
 * @return Pointer to key mapping structure
 */
const multitap_key_t *multitap_get_key_mapping(input_event_t key_event);

#endif // MULTITAP_H
