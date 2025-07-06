#include "menu_bubble.h"
#include "tile.h"
#include "theme.h"
#include "display.h"


void draw_menu_bubble(int tile_x, int tile_y) {
    int px, py;
    tile_to_pixels(tile_x, tile_y, &px, &py);

    int padding = 8;
    int size = TILE_WIDTH - 2 * padding;

    display_fill_rounded_square(px + padding, py + padding, size, size,
                      6, current_theme.fg_colour);
}

void draw_selected_menu_bubble(int tile_x, int tile_y) {
    int px, py;
    tile_to_pixels(tile_x, tile_y, &px, &py);

    display_draw_rounded_square(px + 2, py + 2, TILE_WIDTH - 4, TILE_HEIGHT - 4,
                      8, current_theme.accent_colour);
}
