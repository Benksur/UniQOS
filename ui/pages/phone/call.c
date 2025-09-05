#include "call.h"
#include "screen.h"
#include "display.h"
#include "tile.h"
#include "menu_row.h"
#include "theme.h"
#include "input.h"
#include "cursor.h"
#include <stdlib.h>
#include <string.h>

#define MAX_PHONE_NUMBER_LENGTH 10

typedef enum {
    CALL_STATE_IDLE,
    CALL_STATE_DIALING,
    CALL_STATE_CALLING,
    CALL_STATE_CONNECTED
} CallStatus;

typedef struct {
    Cursor cursor;
    char phone_number[MAX_PHONE_NUMBER_LENGTH + 1];
    CallStatus call_status;
} CallState;


// Forward declarations
static void call_draw(Page* self);
static void call_draw_tile(Page* self, int tile_x, int tile_y);
static void call_handle_input(Page* self, int event_type);
static void call_reset(Page* self);
static void call_destroy(Page* self);
static void add_digit(Page* self, char digit);
static void remove_digit(Page* self);
static void make_call(Page* self);
static void hang_up_call(Page* self);
static void draw_display_area(Page* self);

static void add_digit(Page* self, char digit) {
    CallState* state = (CallState*)self->state;
    if (state->cursor.x < MAX_PHONE_NUMBER_LENGTH) {
        state->phone_number[state->cursor.x] = digit;
        state->cursor.x++;
        state->phone_number[state->cursor.x] = '\0';
        
        // Mark display area dirty for redraw
        mark_tile_dirty(1, 5);
        mark_tile_dirty(2, 5);
        mark_tile_dirty(3, 5);
    }
}

static void clear_old_char(CallState* state) {
    // Calculate position of the digit and cursor to be removed
        int px, py;
        tile_to_pixels(1, 5, &px, &py);
        int digit_x = px + (state->cursor.x - 1) * 6 * 3; // Position of the digit to remove
        int old_cursor_x = px + state->cursor.x * 6 * 3; // Current cursor position
        
        // Clear the specific digit area
        display_fill_rect(digit_x, py, 6 * 3, TILE_HEIGHT, current_theme.highlight_colour);
        
        // Clear the old cursor area
        display_fill_rect(old_cursor_x, py, 5, TILE_HEIGHT - 8, current_theme.highlight_colour);
}

static void remove_digit(Page* self) {
    CallState* state = (CallState*)self->state;
    if (state->cursor.x > 0) {
        clear_old_char(state);
        state->cursor.x--;
        state->phone_number[state->cursor.x] = '\0';
        
        // Mark display area dirty for redraw
        mark_tile_dirty(1, 5);
        mark_tile_dirty(2, 5);
        mark_tile_dirty(3, 5);
    }
}

static void make_call(Page* self) {
    CallState* state = (CallState*)self->state;
    if (state->cursor.x > 0 && state->call_status == CALL_STATE_IDLE) {
        state->call_status = CALL_STATE_CALLING;
        
        // Mark entire screen dirty for redraw
        mark_all_tiles_dirty();
    }
}

static void hang_up_call(Page* self) {
    CallState* state = (CallState*)self->state;
    if (state->call_status != CALL_STATE_IDLE) {
        state->call_status = CALL_STATE_IDLE;
        
        // Mark entire screen dirty for redraw
        mark_all_tiles_dirty();
    }
}

static void draw_call_status(Page* self) {
    CallState* state = (CallState*)self->state;
    const char* status_text = "Enter number:";
    // switch (state->call_status) {
    //     case CALL_STATE_DIALING:
    //         status_text = "Dialing...";
    //         break;
    //     case CALL_STATE_CALLING:
    //         status_text = "Calling...";
    //         break;
    //     case CALL_STATE_CONNECTED:
    //         status_text = "Connected";
    //         break;
    //     default:
    //         status_text = "";
    //         break;
    // }
    int px, py;
    tile_to_pixels(0, 2, &px, &py);
    int width = TILE_WIDTH * TILE_COLS;
    int height = TILE_HEIGHT * 2;
    display_fill_rect(px, py, width, height, current_theme.bg_colour);
    int text_width = strlen(status_text) * 6 * 2; 
    display_draw_string(px + 15, py + 15, status_text, current_theme.fg_colour, current_theme.bg_colour, 2);
    
}

static void draw_number(Page* self) {
    CallState* state = (CallState*)self->state;
    int px, py;
    tile_to_pixels(1, 5, &px, &py);
    
    // Draw the phone number starting from tile (1, 5)
    display_draw_string(px, py, state->phone_number, current_theme.fg_colour, current_theme.highlight_colour, 3);
    
    // Draw cursor after the phone number
    int cursor_x = px + (strlen(state->phone_number) * 6 * 3); // 6 pixels per char * font size 3
    display_fill_rect(cursor_x, py, 5, TILE_HEIGHT - 8, current_theme.fg_colour);
    for (int i = 0; i < TILE_COLS; i++) {
        mark_tile_clean(i, 10);
    }
}

static void call_draw(Page* self) {}

static void call_draw_tile(Page* self, int tile_x, int tile_y) {
    CallState* state = (CallState*)self->state;
    int visible_row = tile_y / 2; // 0-4 on screen
    if (visible_row == 1) {
        draw_call_status(self);
    } else if (visible_row == 2) { 
        // Phone number display area (2 rows)
        // check for tx == 0 to avoid redrawing bg multiple times
        if (tile_x == 0) {
            draw_empty_row_fill(visible_row * 2, current_theme.highlight_colour);
        }
        draw_number(self);
        
        
    } else if (visible_row == 3 && tile_x == 0) {
        int px, py;
        tile_to_pixels(0, visible_row * 2, &px, &py);
        int width = TILE_WIDTH * TILE_COLS;
        int height = TILE_HEIGHT * 3;
        display_fill_rect(px, py, width, height, current_theme.highlight_colour);
    }
    else {
        if (visible_row > 3) return;
        draw_empty_row(visible_row * 2);
    }
    for (int i = 0; i < TILE_COLS; i++) {
        mark_tile_clean(i, visible_row * 2);
        mark_tile_clean(i, visible_row * 2 + 1);
    }

}

static void call_handle_input(Page* self, int event_type) {
    switch (event_type) {
        case INPUT_KEYPAD_0:
            add_digit(self, '0');
            break;
        case INPUT_KEYPAD_1:
            add_digit(self, '1');
            break;
        case INPUT_KEYPAD_2:
            add_digit(self, '2');
            break;
        case INPUT_KEYPAD_3:
            add_digit(self, '3');
            break;
        case INPUT_KEYPAD_4:
            add_digit(self, '4');
            break;
        case INPUT_KEYPAD_5:
            add_digit(self, '5');
            break;
        case INPUT_KEYPAD_6:
            add_digit(self, '6');
            break;
        case INPUT_KEYPAD_7:
            add_digit(self, '7');
            break;
        case INPUT_KEYPAD_8:
            add_digit(self, '8');
            break;
        case INPUT_KEYPAD_9:
            add_digit(self, '9');
            break;
        case INPUT_KEYPAD_STAR:
            add_digit(self, '*');
            break;
        case INPUT_KEYPAD_HASH:
            add_digit(self, '#');
            break;
        case INPUT_PICKUP:
            make_call(self);
            break;
        case INPUT_HANGUP:
            hang_up_call(self);
            break;
        case INPUT_DPAD_LEFT:
            remove_digit(self); // Use left as backspace
            break;
        default:
            // Ignore other inputs
            break;
    }
}

static void call_reset(Page* self) {
    CallState* state = (CallState*)self->state;
    cursor_reset(&state->cursor);
    memset(state->phone_number, 0, sizeof(state->phone_number));
    state->call_status = CALL_STATE_IDLE;
}

static void call_destroy(Page* self) {
    if (self) {
        CallState* state = (CallState*)self->state;
        free(state);
        free(self);
    }
}

Page* call_page_create() {
    Page* page = malloc(sizeof(Page));
    CallState* state = malloc(sizeof(CallState));
    
    // Initialize cursor (x position tracks digit position)
    state->cursor = (Cursor){0, 0, 0, MAX_PHONE_NUMBER_LENGTH - 1, false};
    
    // Initialize phone number and call status
    memset(state->phone_number, 0, sizeof(state->phone_number));
    state->call_status = CALL_STATE_IDLE;
    
    page->draw = call_draw;
    page->draw_tile = call_draw_tile;
    page->handle_input = call_handle_input;
    page->reset = call_reset;
    page->destroy = call_destroy;
    page->state = state;

    return page;
}