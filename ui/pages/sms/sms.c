#include "sms.h"
#include "cursor.h"
#include "screen.h"
#include "display.h"
#include "tile.h"
#include "menu_row.h"
#include "input.h"
#include "theme.h"
#include "new_sms.h"
#include <stdlib.h>
#include <string.h>

#define SMS_OPTIONS_COUNT 3

typedef struct {
    Cursor cursor;
    const char* options[SMS_OPTIONS_COUNT];
} SmsState;


// Forward declarations
static void sms_draw(Page* self);
static void sms_draw_tile(Page* self, int tx, int ty);
static void sms_handle_input(Page* self, int event_type);
static void sms_reset(Page* self);
static void sms_destroy(Page* self);

static void draw_sms_header(int tile_y) {
    int px, py;
    tile_to_pixels(0, tile_y, &px, &py);
    int width = TILE_WIDTH * TILE_COLS;
    int height = TILE_HEIGHT * 2;

    display_fill_rect(px, py, width, height, current_theme.bg_colour);
    
    const char* header_text = "SMS";
    int text_width = strlen(header_text) * 6 * 3; // size 3 font
    int center_x = px + (width - text_width) / 2;
    display_draw_string(center_x, py + 15, header_text, current_theme.fg_colour, current_theme.bg_colour, 3);
    
    // Draw bottom border
    display_draw_horizontal_line(px, py + height - 1, px + width, current_theme.highlight_colour);
}

static void mark_row_dirty(int row) {
    int tile_y = row * 2;
    for (int i = 0; i < TILE_COLS; i++) {
        mark_tile_dirty(i, tile_y);
        mark_tile_dirty(i, tile_y + 1);
    }
}

static void sms_draw(Page* self) {}

static void sms_draw_tile(Page* self, int tx, int ty) {
    SmsState* state = (SmsState*)self->state;
    int visible_row = ty / 2; // 0-4 on screen
    
    if (visible_row == 0) {
        // Header row
        draw_sms_header(0);
    } else if (visible_row <= SMS_OPTIONS_COUNT) {
        // Option rows
        int option_index = visible_row - 1;
        bool highlight = (state->cursor.y == option_index);
        draw_menu_row(visible_row * 2, highlight, state->options[option_index]);
    } else {
        // Empty rows
        draw_empty_row(visible_row * 2);
    }
    
    // Mark tiles as clean
    for (int i = 0; i < TILE_COLS; i++) {
        mark_tile_clean(i, visible_row * 2);
        mark_tile_clean(i, visible_row * 2 + 1);
    }
}

static void sms_handle_input(Page* self, int event_type) {
    SmsState* state = (SmsState*)self->state;
    int old_x, old_y;
    bool moved = false;
    
    // Movement
    switch (event_type) {
        case INPUT_DPAD_UP:
            moved = cursor_move(&state->cursor, 0, -1, &old_x, &old_y);
            break;
        case INPUT_DPAD_DOWN:
            moved = cursor_move(&state->cursor, 0, +1, &old_x, &old_y);
            break;
        case INPUT_SELECT:
            state->cursor.selected = true;
            break;
    }
    
    // Update display if cursor moved
    if (moved) {
        // Mark old and new positions dirty (rows start from 1, not 0 because of header)
        mark_row_dirty(old_y + 1);
        mark_row_dirty(state->cursor.y + 1);
    }
    
    // Handle selection
    if (event_type == INPUT_SELECT) {
        state->cursor.selected = false;
        switch (state->cursor.y) {
            case 0: 
                Page* new_sms_page = new_sms_page_create();
                screen_push_page(new_sms_page);
                break;
            case 1: // Call History
                // TODO: Open call history
                break;
            case 2: // Favourites
                // TODO: Open favourites
                break;
        }
    }
}

static void sms_reset(Page* self) {
    SmsState* state = (SmsState*)self->state;
    cursor_reset(&state->cursor);
}

static void sms_destroy(Page* self) {
    if (self) {
        SmsState* state = (SmsState*)self->state;
        free(state);
        free(self);
    }
}

Page* sms_page_create() {
    Page* page = malloc(sizeof(Page));
    SmsState* state = malloc(sizeof(SmsState));
    
    state->cursor = (Cursor){0, 0, 0, SMS_OPTIONS_COUNT - 1, false};
    
    // Initialize options
    state->options[0] = "New Message";
    state->options[1] = "All Messages";
    state->options[2] = "Favourites";
    
    page->draw = sms_draw;
    page->draw_tile = sms_draw_tile;
    page->handle_input = sms_handle_input;
    page->reset = sms_reset;
    page->destroy = sms_destroy;
    page->state = state;
    
    return page;
}