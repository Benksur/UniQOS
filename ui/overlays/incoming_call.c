#include "incoming_call.h"
#include "memwrap.h"

typedef struct
{
    const char *phone_number;
    IncomingCallCallback callback;
    void *user_data;
} IncomingCallState;

static void incoming_call_draw(Page *self)
{
    IncomingCallState *state = (IncomingCallState *)self->state;

    // Calculate overlay dimensions and position
    int overlay_height = 4;                         // phone number + 2 action buttons + spacing
    int overlay_width = 6;                          // 6 tiles wide for the overlay
    int start_x = (TILE_COLS - overlay_width) / 2;  // Center horizontally
    int start_y = (TILE_ROWS - overlay_height) / 2; // Center vertically

    // Get pixel coordinates for the overlay
    int px, py;
    tile_to_pixels(start_x, start_y, &px, &py);
    int overlay_px_width = overlay_width * TILE_WIDTH;
    int overlay_px_height = overlay_height * TILE_HEIGHT;

    // Clear the entire overlay area
    display_fill_rect(px, py, overlay_px_width, overlay_px_height, current_theme.bg_colour);

    // Draw border around overlay
    display_draw_rect(px, py, overlay_px_width, overlay_px_height, current_theme.accent_colour);

    // Draw "Call from" text (row 0 of overlay)
    int call_from_width = 10 * 6 * 2; // "Call from:" = 10 chars
    int call_from_x = px + (overlay_px_width - call_from_width) / 2;
    int call_from_y = py + (TILE_HEIGHT - 16) / 2;
    display_draw_string(call_from_x, call_from_y, "Call from:",
                        current_theme.text_colour, current_theme.bg_colour, 2);

    // Draw phone number (row 1 of overlay)
    int phone_len = strlen(state->phone_number);
    int phone_width = phone_len * 6 * 2; // 6px per char, size 2
    int phone_x = px + (overlay_px_width - phone_width) / 2;
    int phone_y = py + TILE_HEIGHT + (TILE_HEIGHT - 16) / 2; // Move down one row
    display_draw_string(phone_x, phone_y, state->phone_number,
                        current_theme.text_colour, current_theme.bg_colour, 2);

    // Draw "Pick Up" button (left side)
    tile_to_pixels(1, 5, &px, &py);
    display_draw_string(px + 10, py, "Pick Up",
                        current_theme.fg_colour, current_theme.bg_colour, 1);

    // Draw "Hang Up" button (right side, more to the right)
    tile_to_pixels(5, 5, &px, &py);
    display_draw_string(px + 10, py, "Hang Up",
                        current_theme.fg_colour, current_theme.bg_colour, 1);
}

static void incoming_call_handle_input(Page *self, int event_type)
{
    IncomingCallState *state = (IncomingCallState *)self->state;

    switch (event_type)
    {
    case INPUT_PICKUP:
        // Pick up call (left side)
        if (state->callback)
        {
            state->callback(INCOMING_CALL_ACTION_PICKUP, state->user_data);
            Page *call_page = call_page_create(state->phone_number);
            screen_set_page(call_page);
            screen_handle_response(PAGE_RESPONSE_ACTIVE_CALL, NULL);
        }
        break;
    case INPUT_HANGUP:
        // Hang up call (right side)
        if (state->callback)
        {
            state->callback(INCOMING_CALL_ACTION_HANGUP, state->user_data);
            screen_pop_page();
        }
        break;
    default:
        break;
    }
}

static void incoming_call_reset(Page *self)
{
    // No state to reset for draw-based overlay
    mark_all_tiles_dirty();
}

static void incoming_call_destroy(Page *self)
{
    if (!self)
        return;
    IncomingCallState *state = (IncomingCallState *)self->state;
    mem_free(state);
    mem_free(self);
}

Page *incoming_call_overlay_create(const char *phone_number, IncomingCallCallback callback, void *user_data)
{
    if (!phone_number)
        return NULL;

    Page *page = (Page *)mem_malloc(sizeof(Page));
    IncomingCallState *state = (IncomingCallState *)mem_malloc(sizeof(IncomingCallState));

    state->phone_number = phone_number;
    state->callback = callback;
    state->user_data = user_data;

    page->draw = incoming_call_draw;
    page->draw_tile = NULL;
    page->handle_input = incoming_call_handle_input;
    page->reset = incoming_call_reset;
    page->destroy = incoming_call_destroy;
    page->data_response = NULL;
    page->state = state;

    return page;
}
