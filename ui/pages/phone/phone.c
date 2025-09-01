#include "phone.h"
#include "cursor.h"
#include "screen.h"
#include "display.h"
#include "tile.h"
#include "menu_row.h"
#include "input.h"
#include "theme.h"
#include <stdlib.h>
#include <string.h>

#define PHONE_OPTIONS_COUNT 3

typedef struct {
    Cursor cursor;
    const char* options[PHONE_OPTIONS_COUNT];
} PhoneState;

static Page* current_page = NULL;

// Forward declarations
static void phone_draw();
static void phone_draw_tile(int tx, int ty);
static void phone_handle_input(int event_type);
static void phone_reset();
static void phone_destroy();

static void draw_phone_header(int tile_y) {
    int px, py;
    tile_to_pixels(0, tile_y, &px, &py);
    int width = TILE_WIDTH * TILE_COLS;
    int height = TILE_HEIGHT * 2;

    display_fill_rect(px, py, width, height, current_theme.bg_colour);
    
    const char* header_text = "Phone";
    int text_width = strlen(header_text) * 6 * 3; // size 3 font
    int center_x = px + (width - text_width) / 2;
    display_draw_string(center_x, py + 15, header_text, current_theme.fg_colour, current_theme.bg_colour, 3);
    
    // Draw bottom border
    display_draw_horizontal_line(px, py + height - 1, px + width, current_theme.grid_colour);
}

static void mark_row_dirty(int row) {
    int tile_y = row * 2;
    for (int i = 0; i < TILE_COLS; i++) {
        mark_tile_dirty(i, tile_y);
        mark_tile_dirty(i, tile_y + 1);
    }
}

static void phone_draw() {
    PhoneState* state = (PhoneState*)current_page->state;
    
    // Draw header
    draw_phone_header(0);
    
    // Draw options starting from row 1 (tile_y = 2)
    for (int i = 0; i < PHONE_OPTIONS_COUNT; i++) {
        int tile_y = (i + 1) * 2; // Skip header row
        bool highlight = (state->cursor.y == i);
        draw_menu_row(tile_y, highlight, state->options[i]);
    }
    
    // Draw empty rows for the rest
    for (int i = PHONE_OPTIONS_COUNT + 1; i < 5; i++) {
        draw_empty_row(i * 2);
    }
}

static void phone_draw_tile(int tx, int ty) {
    PhoneState* state = (PhoneState*)current_page->state;
    int visible_row = ty / 2; // 0-4 on screen
    
    if (visible_row == 0) {
        // Header row
        draw_phone_header(0);
    } else if (visible_row <= PHONE_OPTIONS_COUNT) {
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

static void phone_handle_input(int event_type) {
    PhoneState* state = (PhoneState*)current_page->state;
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
            case 0: // Call
                // TODO: Open dialer
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

static void phone_reset() {
    PhoneState* state = (PhoneState*)current_page->state;
    cursor_reset(&state->cursor);
}

static void phone_destroy() {
    if (current_page) {
        PhoneState* state = (PhoneState*)current_page->state;
        free(state);
        free(current_page);
        current_page = NULL;
    }
}

Page* phone_page_create() {
    Page* page = malloc(sizeof(Page));
    PhoneState* state = malloc(sizeof(PhoneState));
    
    // Initialize cursor (0 to PHONE_OPTIONS_COUNT-1)
    state->cursor = (Cursor){0, 0, 0, PHONE_OPTIONS_COUNT - 1, false};
    
    // Initialize options
    state->options[0] = "Call";
    state->options[1] = "Call History";
    state->options[2] = "Favourites";
    
    page->draw = phone_draw;
    page->draw_tile = phone_draw_tile;
    page->handle_input = phone_handle_input;
    page->reset = phone_reset;
    page->destroy = phone_destroy;
    page->state = state;
    
    current_page = page; // Set for static helpers
    return page;
}