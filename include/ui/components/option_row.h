#ifndef OPTION_ROW_H
#define OPTION_ROW_H

#include "tile.h"
#include "theme.h"
#include "display.h"
#include <string.h>

void draw_option_row(int tile_y, int selected, const char* label);
void draw_option_row_highlight_bg(int tile_y, int selected, const char* label);

#endif