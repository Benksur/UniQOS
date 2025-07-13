#include "menu_row.h"
#include "tile.h"
#include "theme.h"
#include "display.h"

void draw_menu_row(int tile_y, int selected)
{
    // Only process even rows
    if (tile_y % 2) return;
    
    int px, py;
    tile_to_pixels(0, tile_y, &px, &py);
    int width = TILE_WIDTH * TILE_COLS;
    int height = TILE_HEIGHT * 2;
    
    // First draw the rectangle background
    if (selected)
    {
        display_fill_rect(px, py, width, height, current_theme.fg_colour);
    } else {
        display_fill_rect(px, py, width, height, current_theme.bg_colour);
    }
    
    // Then draw the rounded square (will be on top of the filled rectangle)
    if (selected)
    {
        display_draw_rounded_square(px + 5, py + 5, 50, 50, 5, current_theme.bg_colour);
    } else {
        display_draw_rounded_square(px + 5, py + 5, 50, 50, 5, current_theme.fg_colour);
    }
    
    // Finally draw the horizontal line (will be on top of everything)
    display_draw_horizontal_line(px, py, px + width, 0x0841);
}
