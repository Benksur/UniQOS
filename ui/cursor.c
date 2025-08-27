#include "cursor.h"

void cursor_init(Cursor* c, int max_x, int max_y) {
    c->x = 0;
    c->y = 0;
    c->max_x = max_x;
    c->max_y = max_y;
    c->selected = 0;
}

bool cursor_move(Cursor* c, int dx, int dy, int* old_x , int* old_y) {
    *old_x = c->x;
    *old_y = c->y;

    c->x += dx;
    if (c->x < 0) c->x = 0;
    if (c->x > c->max_x) c->x = c->max_x;
    
    c->y += dy;
    if (c->y < 0) c->y = 0;
    if (c->y > c->max_y) c->y = c->max_y;

    return (c->x != *old_x || c->y != *old_y);
}

void cursor_select(Cursor* c) {
    c->selected = !c->selected;
}

void cursor_reset(Cursor* c) {
    c->x = 0;
    c->y = 0;
    c->selected = 0;
}