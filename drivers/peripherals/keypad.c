#include <stdbool.h>
#include "keypad.h"
#include "stm32_config.h"
#include "main.h"

// Button state bitmap - each bit represents a button state
static uint32_t button_states[2] = {0}; // 64 buttons max (32 bits per uint32_t)

// Debouncing structure
typedef struct {
    uint32_t last_press_time;
    bool is_pressed;
    bool is_debounced;
} button_debounce_t;

// Button mapping structure
typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
    input_event_t event;
    uint8_t bitmap_index;
    uint8_t bitmap_bit;
} button_map_t;

// Button mapping table
static const button_map_t button_map[] = {
    // Keypad buttons
    {PB_0_GPIO_Port, PB_0_Pin, INPUT_KEYPAD_0, 0, 0},
    {PB_1_GPIO_Port, PB_1_Pin, INPUT_KEYPAD_1, 0, 1},
    {PB_2_GPIO_Port, PB_2_Pin, INPUT_KEYPAD_2, 0, 2},
    {PB_3_GPIO_Port, PB_3_Pin, INPUT_KEYPAD_3, 0, 3},
    {PB_4_GPIO_Port, PB_4_Pin, INPUT_KEYPAD_4, 0, 4},
    {PB_5_GPIO_Port, PB_5_Pin, INPUT_KEYPAD_5, 0, 5},
    {PB_6_GPIO_Port, PB_6_Pin, INPUT_KEYPAD_6, 0, 6},
    {PB_7_GPIO_Port, PB_7_Pin, INPUT_KEYPAD_7, 0, 7},
    {PB_8_GPIO_Port, PB_8_Pin, INPUT_KEYPAD_8, 0, 8},
    {PB_9_GPIO_Port, PB_9_Pin, INPUT_KEYPAD_9, 0, 9},
    {PB_STAR_GPIO_Port, PB_STAR_Pin, INPUT_KEYPAD_STAR, 0, 10},
    {PB_HASH_GPIO_Port, PB_HASH_Pin, INPUT_KEYPAD_HASH, 0, 11},
    
    // D-pad buttons
    {PB_DPAD_UP_GPIO_Port, PB_DPAD_UP_Pin, INPUT_DPAD_UP, 0, 12},
    {PB_DPAD_DOWN_GPIO_Port, PB_DPAD_DOWN_Pin, INPUT_DPAD_DOWN, 0, 13},
    {PB_DPAD_LEFT_GPIO_Port, PB_DPAD_LEFT_Pin, INPUT_DPAD_LEFT, 0, 14},
    {PB_DPAD_RIGHT_GPIO_Port, PB_DPAD_RIGHT_Pin, INPUT_DPAD_RIGHT, 0, 15},
    {PB_DPAD_SELECT_GPIO_Port, PB_DPAD_SELECT_Pin, INPUT_SELECT, 0, 16},
    
    // Menu buttons
    {PB_MENU_L_GPIO_Port, PB_MENU_L_Pin, INPUT_LEFT, 0, 17},
    {PB_MENU_R_GPIO_Port, PB_MENU_R_Pin, INPUT_RIGHT, 0, 18},
    
    // Phone buttons
    {PB_CALL_GPIO_Port, PB_CALL_Pin, INPUT_PICKUP, 0, 19},
    {PB_HANG_GPIO_Port, PB_HANG_Pin, INPUT_HANGUP, 0, 20},
    
    // Volume and power buttons
    {PB_VOL_UP_GPIO_Port, PB_VOL_UP_Pin, INPUT_VOLUME_UP, 0, 21},
    {PB_VOL_DOWN_GPIO_Port, PB_VOL_DOWN_Pin, INPUT_VOLUME_DOWN, 0, 22},
    {PB_PWR_GPIO_Port, PB_PWR_Pin, INPUT_POWER, 0, 23},
};

#define BUTTON_COUNT (sizeof(button_map) / sizeof(button_map[0]))
#define DEBOUNCE_TIME_MS 50  // 50ms debounce time

// Debounce states for each button
static button_debounce_t button_debounce[BUTTON_COUNT] = {0};

void keypad_init(void) {
    // Clear button states
    button_states[0] = 0;
    button_states[1] = 0;
    
    // Initialize debounce states
    for (int i = 0; i < BUTTON_COUNT; i++) {
        button_debounce[i].last_press_time = 0;
        button_debounce[i].is_pressed = false;
        button_debounce[i].is_debounced = false;
    }
}

void keypad_update_states(void) {
    uint32_t current_time = HAL_GetTick();
    
    // Read all button states and update bitmap with debouncing
    for (int i = 0; i < BUTTON_COUNT; i++) {
        const button_map_t* button = &button_map[i];
        button_debounce_t* debounce = &button_debounce[i];
        
        // Read current GPIO state
        bool current_gpio_state = (HAL_GPIO_ReadPin(button->port, button->pin) == GPIO_PIN_RESET);
        
        // Debouncing logic
        if (current_gpio_state != debounce->is_pressed) {
            // State changed, start debounce timer
            debounce->last_press_time = current_time;
            debounce->is_pressed = current_gpio_state;
            debounce->is_debounced = false;
        } else if (current_gpio_state == debounce->is_pressed) {
            // State hasn't changed, check if debounce time has elapsed
            if (!debounce->is_debounced && 
                (current_time - debounce->last_press_time) >= DEBOUNCE_TIME_MS) {
                debounce->is_debounced = true;
            }
        }
        
        // Update bitmap only if debounced
        if (debounce->is_debounced && debounce->is_pressed) {
            button_states[button->bitmap_index] |= (1U << button->bitmap_bit);
        } else {
            button_states[button->bitmap_index] &= ~(1U << button->bitmap_bit);
        }
    }
}

bool keypad_is_button_pressed(uint8_t button_index) {
    if (button_index >= BUTTON_COUNT) {
        return false;
    }
    
    const button_map_t* button = &button_map[button_index];
    return (button_states[button->bitmap_index] & (1U << button->bitmap_bit)) != 0;
}

input_event_t keypad_get_button_event(uint8_t button_index) {
    if (button_index >= BUTTON_COUNT) {
        return INPUT_NONE;
    }
    return button_map[button_index].event;
}

uint8_t keypad_get_button_count(void) {
    return BUTTON_COUNT;
}

// Legacy function for manually scanning
bool keypad_read_button(input_event_t* out_event) {
    // Check for any pressed button
    for (int i = 0; i < BUTTON_COUNT; i++) {
        if (keypad_is_button_pressed(i)) {
            *out_event = button_map[i].event;
            return true;
        }
    }
    return false;
}
