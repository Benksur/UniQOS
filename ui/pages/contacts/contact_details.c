#include "contact_details.h"

typedef struct {
    ContactRecord contact;
    Cursor cursor;
    bool mounted;
} ContactDetailsState;

// Option row labels for contact actions
static const char* option_labels[] = {
    "Call",
    "Send Message",
    "Edit Contact",
    "Delete Contact"
};
#define NUM_OPTIONS (sizeof(option_labels)/sizeof(option_labels[0]))

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
        draw_bottom_bar("Options", "Select", "Back");
        
        state->mounted = true;
    }
    // Draw option rows for ty = 3, 4, 5, 6
    if (ty > 2 && ty < 7) {
        int option_idx = ty - 3;
        if (option_idx >= 0 && option_idx < NUM_OPTIONS) {
            bool highlight = (state->cursor.y == option_idx);
            draw_option_row(ty, highlight, option_labels[option_idx]);
        }
    }
    for (int i = 0; i < TILE_COLS; i++) {
        mark_tile_clean(i, ty);
    }
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
    state->cursor.max_y = NUM_OPTIONS; // Name and Phone
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
    state->cursor.max_y = NUM_OPTIONS;
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
