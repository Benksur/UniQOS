#include "contact_details.h"

typedef struct {
    ContactRecord contact;
    Cursor cursor;
    bool mounted;
} ContactDetailsState;

static void contact_details_draw_tile(Page* self, int tx, int ty);
static void contact_details_handle_input(Page* self, int event_type);
static void contact_details_reset(Page* self);
static void contact_details_destroy(Page* self);

static void draw_header(Page* self) {
    ContactDetailsState* state = (ContactDetailsState*)self->state;
    if (state->contact.name_len > 0) {
        int text_width = state->contact.name_len * 6 * 2; // size 3 font
        int center_x = (TILE_WIDTH * TILE_COLS - text_width) / 2;
        display_draw_string(center_x, 40, state->contact.name, current_theme.text_colour, current_theme.bg_colour, 2);
    } else {
        char* title = "(Name)";
        int text_width = strlen(title) * 6 * 2; // size 3 font
        int center_x = (TILE_WIDTH * TILE_COLS - text_width) / 2;
        display_draw_string(center_x, 40, title, current_theme.text_colour, current_theme.bg_colour, 2);
    }
}

static void contact_details_draw_tile(Page* self, int tx, int ty) {
    ContactDetailsState* state = (ContactDetailsState*)self->state;
    if (!state->mounted) {
        display_fill_rect(0, 30, TILE_WIDTH * TILE_COLS, TILE_HEIGHT * (TILE_ROWS + 1), current_theme.bg_colour);
        draw_header(self);

        // Draw phone number
        if (state->contact.phone_len > 0) {
            int text_width = state->contact.phone_len * 6 * 2; // size 3 font
            int center_x = (TILE_WIDTH * TILE_COLS - text_width) / 2;
            display_draw_string(center_x, 80, state->contact.phone, current_theme.text_colour, current_theme.bg_colour, 2);
        } else {
            char* title = "(Phone)";
            int text_width = strlen(title) * 6 * 2; // size 3 font
            int center_x = (TILE_WIDTH * TILE_COLS - text_width) / 2;
            display_draw_string(center_x, 80, title, current_theme.text_colour, current_theme.bg_colour, 2);
        }

        state->mounted = true;
    }

    // Draw cursor
    int cursor_x = state->cursor.x * TILE_WIDTH;
    int cursor_y = state->cursor.y * TILE_HEIGHT + 30; // Offset for header
    display_draw_rect(cursor_x, cursor_y, TILE_WIDTH, TILE_HEIGHT, current_theme.accent_colour);
}

static void contact_details_handle_input(Page* self, int event_type) {
    ContactDetailsState* state = (ContactDetailsState*)self->state;
    switch (event_type) {
        case INPUT_DPAD_UP:
            if (state->cursor.y > 0) state->cursor.y--;
            break;
        case INPUT_DPAD_DOWN:
            if (state->cursor.y < state->cursor.max_y - 1) state->cursor.y++;
            break;
        case INPUT_DPAD_LEFT:
            if (state->cursor.x > 0) state->cursor.x--;
            break;
        case INPUT_DPAD_RIGHT:
            if (state->cursor.x < state->cursor.max_x - 1) state->cursor.x++;
            break;
        case INPUT_KEYPAD_0: // Back
            // Go back to previous page
            // This would typically involve a page stack in a full implementation
            break;
        default:
            break;
    }
    mark_all_tiles_dirty();
}

static void contact_details_reset(Page* self) {
    ContactDetailsState* state = (ContactDetailsState*)self->state;
    state->cursor.x = 0;
    state->cursor.y = 0;
    state->cursor.max_x = 1; // Only one column for now
    state->cursor.max_y = 2; // Name and Phone
    state->mounted = false;
    mark_all_tiles_dirty();
}

static void contact_details_destroy(Page* self) {
    if (self == NULL) return;
    ContactDetailsState* state = (ContactDetailsState*)self->state;
    free(state);
    free(self);
}

Page* contact_details_page_create(ContactRecord contact) {
    Page* page = (Page*)malloc(sizeof(Page));
    ContactDetailsState* state = (ContactDetailsState*)malloc(sizeof(ContactDetailsState));

    if (contact.name_len > 0) {
        state->contact = contact;
    } else {
        // Optionally, zero out state->contact or set defaults
        memset(&state->contact, 0, sizeof(ContactRecord));
    }
    state->cursor.x = 0;
    state->cursor.y = 0;
    state->cursor.max_x = 1;
    state->cursor.max_y = 2;
    state->mounted = false;

    page->draw = NULL; // Full redraw not needed, using tile redraw
    page->draw_tile = contact_details_draw_tile;
    page->handle_input = contact_details_handle_input;
    page->reset = contact_details_reset;
    page->destroy = contact_details_destroy;
    page->data_response = NULL; // No data response needed
    page->state = state;

    return page;
}
