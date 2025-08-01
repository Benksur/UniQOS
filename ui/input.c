#include <stdbool.h>
#include "input.h"
#include "screen.h"
#include "drivers/peripherals/keypad.h"

bool read_keypad(input_event_t* out_event) {
    return keypad_read_button(out_event);
}

input_event_t input_poll(void) {
    input_event_t event;
    if (read_keypad(&event)) {
        return event; 
    }
    return INPUT_NONE;
}

void input_on_key(input_event_t event) {
    screen_handle_input(event);
}
