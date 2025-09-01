#ifndef CURSOR_H
#define CURSOR_H

#include <stdbool.h>

typedef struct {
    int x;
    int y;
    int max_x;
    int max_y;
    int selected;
} Cursor;

void cursor_init(Cursor* c, int max_x, int max_y);
bool cursor_move(Cursor* c, int dx, int dy, int* old_x, int* old_y);
void cursor_select(Cursor* c);
void cursor_reset(Cursor* c);

#endif