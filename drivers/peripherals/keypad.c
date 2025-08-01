#include <stdbool.h>
#include "keypad.h"
#include "stm32_config.h"
#include "main.h"

bool keypad_read_button(input_event_t* out_event) {
    // Check keypad buttons
    if (HAL_GPIO_ReadPin(PB_0_GPIO_Port, PB_0_Pin) == GPIO_PIN_RESET) {
        *out_event = INPUT_KEYPAD_0;
        return true;
    }
    if (HAL_GPIO_ReadPin(PB_1_GPIO_Port, PB_1_Pin) == GPIO_PIN_RESET) {
        *out_event = INPUT_KEYPAD_1;
        return true;
    }
    if (HAL_GPIO_ReadPin(PB_2_GPIO_Port, PB_2_Pin) == GPIO_PIN_RESET) {
        *out_event = INPUT_KEYPAD_2;
        return true;
    }
    if (HAL_GPIO_ReadPin(PB_3_GPIO_Port, PB_3_Pin) == GPIO_PIN_RESET) {
        *out_event = INPUT_KEYPAD_3;
        return true;
    }
    if (HAL_GPIO_ReadPin(PB_4_GPIO_Port, PB_4_Pin) == GPIO_PIN_RESET) {
        *out_event = INPUT_KEYPAD_4;
        return true;
    }
    if (HAL_GPIO_ReadPin(PB_5_GPIO_Port, PB_5_Pin) == GPIO_PIN_RESET) {
        *out_event = INPUT_KEYPAD_5;
        return true;
    }
    if (HAL_GPIO_ReadPin(PB_6_GPIO_Port, PB_6_Pin) == GPIO_PIN_RESET) {
        *out_event = INPUT_KEYPAD_6;
        return true;
    }
    if (HAL_GPIO_ReadPin(PB_7_GPIO_Port, PB_7_Pin) == GPIO_PIN_RESET) {
        *out_event = INPUT_KEYPAD_7;
        return true;
    }
    if (HAL_GPIO_ReadPin(PB_8_GPIO_Port, PB_8_Pin) == GPIO_PIN_RESET) {
        *out_event = INPUT_KEYPAD_8;
        return true;
    }
    if (HAL_GPIO_ReadPin(PB_9_GPIO_Port, PB_9_Pin) == GPIO_PIN_RESET) {
        *out_event = INPUT_KEYPAD_9;
        return true;
    }
    if (HAL_GPIO_ReadPin(PB_STAR_GPIO_Port, PB_STAR_Pin) == GPIO_PIN_RESET) {
        *out_event = INPUT_KEYPAD_STAR;
        return true;
    }
    if (HAL_GPIO_ReadPin(PB_HASH_GPIO_Port, PB_HASH_Pin) == GPIO_PIN_RESET) {
        *out_event = INPUT_KEYPAD_HASH;
        return true;
    }
    
    // Check D-pad buttons
    if (HAL_GPIO_ReadPin(PB_DPAD_UP_GPIO_Port, PB_DPAD_UP_Pin) == GPIO_PIN_RESET) {
        *out_event = INPUT_DPAD_UP;
        return true;
    }
    if (HAL_GPIO_ReadPin(PB_DPAD_DOWN_GPIO_Port, PB_DPAD_DOWN_Pin) == GPIO_PIN_RESET) {
        *out_event = INPUT_DPAD_DOWN;
        return true;
    }
    if (HAL_GPIO_ReadPin(PB_DPAD_LEFT_GPIO_Port, PB_DPAD_LEFT_Pin) == GPIO_PIN_RESET) {
        *out_event = INPUT_DPAD_LEFT;
        return true;
    }
    if (HAL_GPIO_ReadPin(PB_DPAD_RIGHT_GPIO_Port, PB_DPAD_RIGHT_Pin) == GPIO_PIN_RESET) {
        *out_event = INPUT_DPAD_RIGHT;
        return true;
    }
    if (HAL_GPIO_ReadPin(PB_DPAD_SELECT_GPIO_Port, PB_DPAD_SELECT_Pin) == GPIO_PIN_RESET) {
        *out_event = INPUT_SELECT;
        return true;
    }
    
    // Check menu buttons
    if (HAL_GPIO_ReadPin(PB_MENU_L_GPIO_Port, PB_MENU_L_Pin) == GPIO_PIN_RESET) {
        *out_event = INPUT_LEFT;
        return true;
    }
    if (HAL_GPIO_ReadPin(PB_MENU_R_GPIO_Port, PB_MENU_R_Pin) == GPIO_PIN_RESET) {
        *out_event = INPUT_RIGHT;
        return true;
    }
    
    // Check phone buttons
    if (HAL_GPIO_ReadPin(PB_CALL_GPIO_Port, PB_CALL_Pin) == GPIO_PIN_RESET) {
        *out_event = INPUT_PICKUP;
        return true;
    }
    if (HAL_GPIO_ReadPin(PB_HANG_GPIO_Port, PB_HANG_Pin) == GPIO_PIN_RESET) {
        *out_event = INPUT_HANGUP;
        return true;
    }
    
    // Check volume and power buttons
    if (HAL_GPIO_ReadPin(PB_VOL_UP_GPIO_Port, PB_VOL_UP_Pin) == GPIO_PIN_RESET) {
        *out_event = INPUT_VOLUME_UP;
        return true;
    }
    if (HAL_GPIO_ReadPin(PB_VOL_DOWN_GPIO_Port, PB_VOL_DOWN_Pin) == GPIO_PIN_RESET) {
        *out_event = INPUT_VOLUME_DOWN;
        return true;
    }
    if (HAL_GPIO_ReadPin(PB_PWR_GPIO_Port, PB_PWR_Pin) == GPIO_PIN_RESET) {
        *out_event = INPUT_POWER;
        return true;
    }
    
    return false;
}
