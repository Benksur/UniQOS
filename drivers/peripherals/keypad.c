/**
 * @file keypad.c
 * @brief Keypad input driver implementation
 *
 * This driver handles keypad and button input with debouncing support.
 * It manages both keypad buttons (0-9, *, #) and other input buttons
 * including D-pad, menu, phone, volume, and power buttons.
 */

#include <stdbool.h>
#include "keypad.h"
#include "stm32_config.h"
#include "main.h"

// Button state bitmap - each bit represents a button state
static uint32_t button_states;
static uint32_t button_states_last;
static uint32_t button_states_edge;

// Debouncing structure
typedef struct
{
    uint32_t last_press_time;
    bool is_pressed;
    bool is_debounced;
} button_debounce_t;

// Button mapping structure
typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
    input_event_t event;
    uint8_t bitmap_bit;
} button_map_t;

// Button mapping table
static const button_map_t button_map[] = {
    // Keypad buttons
    {PB_0_GPIO_Port, PB_0_Pin, INPUT_KEYPAD_0, 0},
    {PB_1_GPIO_Port, PB_1_Pin, INPUT_KEYPAD_1, 1},
    {PB_2_GPIO_Port, PB_2_Pin, INPUT_KEYPAD_2, 2},
    {PB_3_GPIO_Port, PB_3_Pin, INPUT_KEYPAD_3, 3},
    {PB_4_GPIO_Port, PB_4_Pin, INPUT_KEYPAD_4, 4},
    {PB_5_GPIO_Port, PB_5_Pin, INPUT_KEYPAD_5, 5},
    {PB_6_GPIO_Port, PB_6_Pin, INPUT_KEYPAD_6, 6},
    {PB_7_GPIO_Port, PB_7_Pin, INPUT_KEYPAD_7, 7},
    {PB_8_GPIO_Port, PB_8_Pin, INPUT_KEYPAD_8, 8},
    {PB_9_GPIO_Port, PB_9_Pin, INPUT_KEYPAD_9, 9},
    {PB_STAR_GPIO_Port, PB_STAR_Pin, INPUT_KEYPAD_STAR, 10},
    {PB_HASH_GPIO_Port, PB_HASH_Pin, INPUT_KEYPAD_HASH, 11},

    // D-pad buttons
    {PB_DPAD_UP_GPIO_Port, PB_DPAD_UP_Pin, INPUT_DPAD_UP, 12},
    {PB_DPAD_DOWN_GPIO_Port, PB_DPAD_DOWN_Pin, INPUT_DPAD_DOWN, 13},
    {PB_DPAD_LEFT_GPIO_Port, PB_DPAD_LEFT_Pin, INPUT_DPAD_LEFT, 14},
    {PB_DPAD_RIGHT_GPIO_Port, PB_DPAD_RIGHT_Pin, INPUT_DPAD_RIGHT, 15},
    {PB_DPAD_SELECT_GPIO_Port, PB_DPAD_SELECT_Pin, INPUT_SELECT, 16},

    // Menu buttons
    {PB_MENU_L_GPIO_Port, PB_MENU_L_Pin, INPUT_LEFT, 17},
    {PB_MENU_R_GPIO_Port, PB_MENU_R_Pin, INPUT_RIGHT, 18},

    // Phone buttons
    {PB_CALL_GPIO_Port, PB_CALL_Pin, INPUT_PICKUP, 19},
    {PB_END_CALL_GPIO_Port, PB_END_CALL_Pin, INPUT_HANGUP, 20},

    // Volume and power buttons
    {PB_VOL_UP_GPIO_Port, PB_VOL_UP_Pin, INPUT_VOLUME_UP, 21},
    {PB_VOL_DOWN_GPIO_Port, PB_VOL_DOWN_Pin, INPUT_VOLUME_DOWN, 22},
    {PB_PWR_GPIO_Port, PB_PWR_Pin, INPUT_POWER, 23},
};

#define BUTTON_COUNT (sizeof(button_map) / sizeof(button_map[0]))
#define DEBOUNCE_TIME_MS 30

// Debounce states for each button
static button_debounce_t button_debounce[BUTTON_COUNT] = {0};

/**
 * @brief Initialize the keypad driver
 */
void keypad_init(void)
{
    // clear button states
    button_states = 0;
    button_states_last = 0;
    button_states_edge = 0;
    // initialize debounce states
    for (int i = 0; i < BUTTON_COUNT; i++)
    {
        button_debounce[i].last_press_time = 0;
        button_debounce[i].is_pressed = false;
        button_debounce[i].is_debounced = false;
    }
}

/**
 * @brief Update button states with debouncing
 *
 * This function should be called periodically to read and debounce all button states.
 * It updates the internal button state bitmap and edge detection.
 */
void keypad_update_states(void)
{
    uint32_t current_time = HAL_GetTick();
    uint32_t current_states = 0;

    // read all button states and update bitmap with debouncing
    for (int i = 0; i < BUTTON_COUNT; i++)
    {
        const button_map_t *button = &button_map[i];
        button_debounce_t *debounce = &button_debounce[i];

        // read current gpio state
        bool current_gpio_state = (HAL_GPIO_ReadPin(button->port, button->pin) == GPIO_PIN_RESET);

        // debouncing logic
        if (current_gpio_state != debounce->is_pressed)
        {
            // state changed, start debounce timer
            debounce->last_press_time = current_time;
            debounce->is_pressed = current_gpio_state;
            debounce->is_debounced = false;
        }
        else if (current_gpio_state == debounce->is_pressed)
        {
            // state hasn't changed, check if debounce time has elapsed
            if (!debounce->is_debounced &&
                (current_time - debounce->last_press_time) >= DEBOUNCE_TIME_MS)
            {
                debounce->is_debounced = true;
            }
        }

        // update bitmap only if debounced
        if (debounce->is_debounced && debounce->is_pressed)
        {
            current_states |= (1U << button->bitmap_bit);
        }
    }
    uint32_t edge = current_states & ~button_states_last;
    button_states_edge |= edge;
    button_states_last = current_states;
    button_states = current_states;
}

/**
 * @brief Check if a button was pressed (edge detection)
 * @param button_index Index of button to check (0 to BUTTON_COUNT-1)
 * @return true if button was pressed since last check, false otherwise
 */
bool keypad_is_button_pressed(uint8_t button_index)
{
    if (button_index >= BUTTON_COUNT)
    {
        return false;
    }
    uint32_t mask = (1U << button_map[button_index].bitmap_bit);
    bool was = (button_states_edge & mask) != 0;
    button_states_edge &= ~mask; // clear the edge bit
    return was;
}

/**
 * @brief Get the input event associated with a button
 * @param button_index Index of button (0 to BUTTON_COUNT-1)
 * @return Input event type for the button, or INPUT_NONE if invalid index
 */
input_event_t keypad_get_button_event(uint8_t button_index)
{
    if (button_index >= BUTTON_COUNT)
    {
        return INPUT_NONE;
    }
    return button_map[button_index].event;
}

/**
 * @brief Get the total number of buttons
 * @return Number of buttons managed by the keypad driver
 */
uint8_t keypad_get_button_count(void)
{
    return BUTTON_COUNT;
}

/**
 * @brief Legacy function for manually scanning buttons
 * @param out_event Pointer to store the input event of pressed button
 * @return true if a button was pressed, false otherwise
 * @note This function checks all buttons and returns the first pressed one
 */
bool keypad_read_button(input_event_t *out_event)
{
    // check for any pressed button
    for (int i = 0; i < BUTTON_COUNT; i++)
    {
        if (keypad_is_button_pressed(i))
        {
            *out_event = button_map[i].event;
            return true;
        }
    }
    return false;
}
