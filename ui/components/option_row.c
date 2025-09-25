#include "option_row.h"

void draw_option_row(int tile_y, int selected, const char* label) {
    int px, py;
    tile_to_pixels(0, tile_y, &px, &py);
    int width = TILE_WIDTH * TILE_COLS;
    int height = TILE_HEIGHT;

    // Center the label text using fixed-width font assumption (6px per char, size 2)
    int label_len = strlen(label);
    int label_width = label_len * 6 * 2;
    int text_x = px + (width - label_width) / 2;
    int text_y = py + (height - 16) / 2; // assuming 16px font height for size 2

    if (selected) {
        display_fill_rect(px, py, width, height, current_theme.highlight_colour);
        display_draw_string(text_x, text_y, label, current_theme.fg_colour, current_theme.highlight_colour, 2);
    } else {
        display_fill_rect(px, py, width, height, current_theme.bg_colour);
        display_draw_string(text_x, text_y, label, current_theme.fg_colour, current_theme.bg_colour, 2);
    }

    // Draw lines at the top and bottom
    display_draw_horizontal_line(px, py, px + width, current_theme.highlight_colour);
    display_draw_horizontal_line(px, py + height - 1, px + width, current_theme.highlight_colour);
}

void draw_option_row_highlight_bg(int tile_y, int selected, const char* label) {
    int px, py;
    tile_to_pixels(0, tile_y, &px, &py);
    int width = TILE_WIDTH * TILE_COLS;
    int height = TILE_HEIGHT;

    int label_len = strlen(label);
    int label_width = label_len * 6 * 2;
    int text_x = px + (width - label_width) / 2;
    int text_y = py + (height - 16) / 2;

    if (selected) {
        display_fill_rect(px, py, width, height, current_theme.bg_colour);
        display_draw_string(text_x, text_y, label, current_theme.text_colour, current_theme.bg_colour, 2);
    } else {
        display_fill_rect(px, py, width, height, current_theme.highlight_colour);
        display_draw_string(text_x, text_y, label, current_theme.text_colour, current_theme.highlight_colour, 2);
    }
}