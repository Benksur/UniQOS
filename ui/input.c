#include <stdbool.h>
#include "input.h"
#include "screen.h"

#define PIN_KEY_UP        0x01 // Placeholder for actual pin number
#define PIN_KEY_DOWN      0x02 // Placeholder for actual pin number
#define PIN_KEY_SELECT    0x03 // Placeholder for actual pin number

bool is_button_pressed(int pin) {
    // Placeholder for actual GPIO read logic
    // This should return true if the button connected to the pin is pressed
    return false; // Replace with actual logic
}

bool read_keypad(InputEvent* out_event) {
    // placeholders for actual button logic
    if (is_button_pressed(PIN_KEY_UP)) {
        *out_event = INPUT_UP;
        return true;
    }
    if (is_button_pressed(PIN_KEY_SELECT)) {
        *out_event = INPUT_SELECT;
        return true;
    }
    return false;
}

void input_poll(void) {
    InputEvent event;
    if (read_keypad(&event)) {
        input_on_key(event);
    }
}

void input_on_key(InputEvent event) {
    screen_handle_input((int)event, 0, 0);
}
