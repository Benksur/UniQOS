#include "menu_row.h"
#include "tile.h"
#include "theme.h"
#include "display.h"



void draw_menu_row(int tile_y, int selected, const char* text)
{
    if (tile_y % 2) return;
    int px, py;
    tile_to_pixels(0, tile_y, &px, &py);
    int width = TILE_WIDTH * TILE_COLS;
    int height = TILE_HEIGHT * 2;
    
    if (selected)
    {
        display_fill_rect(px, py, width, height, current_theme.fg_colour);
        display_draw_string(px + 60, py + 10, text, current_theme.bg_colour, current_theme.fg_colour, 2);
        display_draw_rounded_square(px + 5, py + 5, 50, 50, 5, current_theme.bg_colour);
    } else {
        display_fill_rect(px, py, width, height, current_theme.bg_colour);
        display_draw_string(px + 60, py + 10, text, current_theme.text_colour, current_theme.bg_colour, 2);
        display_draw_rounded_square(px + 5, py + 5, 50, 50, 5, current_theme.fg_colour);
    }

    display_draw_horizontal_line(px, py, px + width, current_theme.grid_colour);
}
