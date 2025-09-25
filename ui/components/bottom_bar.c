#include "bottom_bar.h"

void draw_bottom_bar(const char* left, const char* center, const char* right) {
    int bar_y = 25 + TILE_HEIGHT * TILE_ROWS;
    int bar_height = TILE_HEIGHT;
    int bar_width = TILE_WIDTH * TILE_COLS;
    display_fill_rect(0, bar_y, bar_width, bar_height, current_theme.fg_colour);

    // Font size and metrics
    int font_size = 1;
    int char_width = 6 * font_size;
    int char_height = 16; // for size 2

    // Left label
    if (left && left[0]) {
        display_draw_string(15, bar_y + (bar_height - char_height) / 2, left, current_theme.bg_colour, current_theme.fg_colour, font_size);
    }
    // Center label
    if (center && center[0]) {
        int center_width = strlen(center) * char_width;
        int center_x = (bar_width - center_width) / 2;
        display_draw_string(center_x, bar_y + (bar_height - char_height) / 2, center, current_theme.bg_colour, current_theme.fg_colour, font_size);
    }
    // Right label
    if (right && right[0]) {
        int right_width = strlen(right) * char_width;
        int right_x = bar_width - right_width - 20;
        display_draw_string(right_x, bar_y + (bar_height - char_height) / 2, right, current_theme.bg_colour, current_theme.fg_colour, font_size);
    }
}