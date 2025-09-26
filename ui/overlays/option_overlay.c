#include "option_overlay.h"
#include "cursor.h"

#define MAX_OPTIONS 3

typedef struct {
    const char* header;
    const char** options;
    int num_options;
    Cursor cursor;
    OptionOverlayCallback callback;
    void* user_data;
    bool mounted;
} OptionOverlayState;

static void option_overlay_draw_tile(Page* self, int tx, int ty) {
    OptionOverlayState* state = (OptionOverlayState*)self->state;
    int overlay_height = 1 + state->num_options; // header + options
    int y_offset = TILE_ROWS - overlay_height; // Start drawing from the bottom
    if (!state->mounted) {
        // Clear overlay area (header + options) at the bottom
        display_fill_rect(0, (y_offset + 1) * TILE_HEIGHT, TILE_WIDTH * TILE_COLS, TILE_HEIGHT * overlay_height - 5, current_theme.highlight_colour);
        state->mounted = true;
    }
    // Draw header row using the standard option row highlight bg, but with the header text
    if (ty == y_offset) {
        draw_option_row_highlight_bg(ty, false, state->header);
        int px, py;
        tile_to_pixels(0, ty, &px, &py);
        display_draw_horizontal_line(0, py - 1, TILE_WIDTH * TILE_COLS, current_theme.text_colour);
        display_draw_horizontal_line(0, py + TILE_HEIGHT - 1, TILE_WIDTH * TILE_COLS, current_theme.text_colour);
    }

    // Draw option rows (tiles y_offset+1 .. y_offset+num_options)
    if (ty > y_offset && ty <= y_offset + state->num_options) {
        int option_idx = ty - (y_offset + 1);
        if (option_idx >= 0 && option_idx < state->num_options) {
            draw_option_row_highlight_bg(ty, state->cursor.y == option_idx, state->options[option_idx]);
        }
    }
    for (int i = 0; i < TILE_COLS; i++) {
        mark_tile_clean(i, ty);
    }
}

static void option_overlay_handle_input(Page* self, int event_type) {
    OptionOverlayState* state = (OptionOverlayState*)self->state;
    int old_x, old_y;
    bool moved = false;
    switch (event_type) {
        case INPUT_DPAD_UP:
            moved = cursor_move(&state->cursor, 0, -1, &old_x, &old_y);
            break;
        case INPUT_DPAD_DOWN:
            moved = cursor_move(&state->cursor, 0, +1, &old_x, &old_y);
            break;
        default:
            break;
    }
    if (moved) {
        int overlay_height = 1 + state->num_options;
        int y_offset = TILE_ROWS - overlay_height;
        int old_row = y_offset + 1 + old_y;
        int new_row = y_offset + 1 + state->cursor.y;
        if (old_row >= y_offset + 1 && old_row < y_offset + 1 + state->num_options)
            mark_tile_dirty(0, old_row);
        if (new_row >= y_offset + 1 && new_row < y_offset + 1 + state->num_options && new_row != old_row)
            mark_tile_dirty(0, new_row);
    }
    if (event_type == INPUT_SELECT) {
        if (state->callback) state->callback(state->cursor.y, state->user_data);
    } else if (event_type == INPUT_KEYPAD_0) {
        if (state->callback) state->callback(-1, state->user_data);
    }
}

static void option_overlay_reset(Page* self) {
    OptionOverlayState* state = (OptionOverlayState*)self->state;
    state->cursor.x = 0;
    state->cursor.y = 0;
    state->cursor.max_x = 1;
    state->cursor.max_y = state->num_options - 1;
    state->cursor.selected = 0;
    state->mounted = false;
    mark_all_tiles_dirty();
}

static void option_overlay_destroy(Page* self) {
    if (!self) return;
    OptionOverlayState* state = (OptionOverlayState*)self->state;
    free(state);
    free(self);
}

Page* option_overlay_page_create(const char* header, const char** options, int num_options, OptionOverlayCallback callback, void* user_data) {
    if (num_options < 1 || num_options > MAX_OPTIONS) return NULL;
    Page* page = (Page*)malloc(sizeof(Page));
    OptionOverlayState* state = (OptionOverlayState*)malloc(sizeof(OptionOverlayState));
    state->header = header;
    state->options = options;
    state->num_options = num_options;
    state->cursor.x = 0;
    state->cursor.y = 0;
    state->cursor.max_x = 1;
    state->cursor.max_y = num_options - 1;
    state->cursor.selected = 0;
    state->callback = callback;
    state->user_data = user_data;
    state->mounted = false;
    page->draw = NULL;
    page->draw_tile = option_overlay_draw_tile;
    page->handle_input = option_overlay_handle_input;
    page->reset = option_overlay_reset;
    page->destroy = option_overlay_destroy;
    page->data_response = NULL;
    page->state = state;
    return page;
}
