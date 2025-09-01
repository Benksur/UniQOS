#ifndef MENU_ROW_H
#define MENU_ROW_H

#include <stdint.h>

void draw_menu_row(int tile_y, int selected, const char* text);
void draw_empty_row(int tile_y);
void draw_empty_row_fill(int tile_y, uint16_t colour);

#endif
