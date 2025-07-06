#ifndef INPUT_H
#define INPUT_H

typedef enum {
    INPUT_UP,
    INPUT_DOWN,
    INPUT_LEFT,
    INPUT_RIGHT,
    INPUT_SELECT,
    INPUT_BACK
} InputEvent;

void input_poll(void);
void input_on_key(InputEvent event);

#endif
