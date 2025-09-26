#include "bottom_bar.h"

void draw_bottom_bar(const char* left, const char* center, const char* right, int accent_index) {
    int bar_y = 25 + TILE_HEIGHT * TILE_ROWS;
    int bar_height = TILE_HEIGHT;
    int bar_width = TILE_WIDTH * TILE_COLS;
    display_fill_rect(0, bar_y, bar_width, bar_height, current_theme.fg_colour);

    int font_size = 1;
    int char_width = 6 * font_size;
    int char_height = 16; // for size 2

    // Draw left label
    if (left && left[0]) {
        int left_width = strlen(left) * char_width;
        int left_x = 15;
        int left_y = bar_y + (bar_height - char_height) / 2;
        uint16_t bg = (accent_index == 1) ? current_theme.accent_colour : current_theme.fg_colour;
        if (accent_index == 1) {
            display_fill_rect(left_x - 4, left_y - 4, left_width + 8, char_height, current_theme.accent_colour);
        }
        display_draw_string(left_x, left_y, left, current_theme.bg_colour, bg, font_size);
    }
    // Draw center label
    if (center && center[0]) {
        int center_width = strlen(center) * char_width;
        int center_x = (bar_width - center_width) / 2;
        int center_y = bar_y + (bar_height - char_height) / 2;
        uint16_t bg = (accent_index == 2) ? current_theme.accent_colour : current_theme.fg_colour;
        if (accent_index == 2) {
            display_fill_rect(center_x - 4, center_y - 4, center_width + 8, char_height, current_theme.accent_colour);
        }
        display_draw_string(center_x, center_y, center, current_theme.bg_colour, bg, font_size);
    }
    // Draw right label
    if (right && right[0]) {
        int right_width = strlen(right) * char_width;
        int right_x = bar_width - right_width - 20;
        int right_y = bar_y + (bar_height - char_height) / 2;
        uint16_t bg = (accent_index == 3) ? current_theme.accent_colour : current_theme.fg_colour;
        if (accent_index == 3) {
            display_fill_rect(right_x - 4, right_y - 4, right_width + 8, char_height, current_theme.accent_colour);
        }
        display_draw_string(right_x, right_y, right, current_theme.bg_colour, bg, font_size);
    }
}