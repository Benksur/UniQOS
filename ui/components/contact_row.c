#include "contact_row.h"

void draw_contact_row(int tile_y, int selected, const char* name) {
    int px, py;
    tile_to_pixels(0, tile_y, &px, &py);
    int width = TILE_WIDTH * TILE_COLS;
    int height = TILE_HEIGHT;

    if (selected) {
        display_fill_rect(px, py, width, height, current_theme.highlight_colour);
        display_draw_string(px + 10, py + 10, name, current_theme.text_colour, current_theme.highlight_colour, 2);
    } else {
        display_fill_rect(px, py, width, height, current_theme.bg_colour);
        display_draw_string(px + 10, py + 10, name, current_theme.text_colour, current_theme.bg_colour, 2);

    }


    display_draw_horizontal_line(px, py + height - 1, px + width, current_theme.highlight_colour);
}