#ifndef INPUT_H
#define INPUT_H

typedef enum {
    INPUT_NONE,
    INPUT_DPAD_UP,
    INPUT_DPAD_DOWN,
    INPUT_DPAD_LEFT,
    INPUT_DPAD_RIGHT,
    INPUT_SELECT,
    INPUT_LEFT,
    INPUT_RIGHT,
    INPUT_PICKUP,
    INPUT_HANGUP,
    INPUT_POWER,
    INPUT_VOLUME_UP,
    INPUT_VOLUME_DOWN,
    INPUT_KEYPAD_0,
    INPUT_KEYPAD_1,
    INPUT_KEYPAD_2,
    INPUT_KEYPAD_3,
    INPUT_KEYPAD_4,
    INPUT_KEYPAD_5,
    INPUT_KEYPAD_6,
    INPUT_KEYPAD_7,
    INPUT_KEYPAD_8,
    INPUT_KEYPAD_9,
    INPUT_KEYPAD_STAR,
    INPUT_KEYPAD_HASH,
} input_event_t;

void input_poll(void);
void input_on_key(input_event_t event);

#endif
