#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdbool.h>
#include "ui/input.h"

bool keypad_read_button(input_event_t* out_event);

#endif // KEYPAD_H 