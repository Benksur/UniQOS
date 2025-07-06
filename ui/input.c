#include <stdbool.h>
#include "input.h"
#include "screen.h"

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
