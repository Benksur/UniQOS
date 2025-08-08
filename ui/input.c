#include <stdbool.h>
#include "input.h"
#include "screen.h"
#include "keypad.h"

bool read_keypad(input_event_t* out_event) {
    return keypad_read_button(out_event);
}

void input_poll(void) {
    input_event_t event;
    if (read_keypad(&event)) {
        input_on_key(event);
    }
}

void input_on_key(input_event_t event) {
    screen_handle_input(event);
}
