#include "incoming_text.h"
#include "memwrap.h"

typedef struct
{
    const char *sender_number;
    IncomingTextCallback callback;
    void *user_data;
} IncomingTextState;

static void incoming_text_draw(Page *self)
{
    IncomingTextState *state = (IncomingTextState *)self->state;

    // Calculate overlay dimensions and position
    int overlay_height = 4;                         // sender number + 2 action buttons + spacing
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

    // Draw "Text from" text (row 0 of overlay)
    int text_from_width = 10 * 6 * 2; // "Text from:" = 10 chars
    int text_from_x = px + (overlay_px_width - text_from_width) / 2;
    int text_from_y = py + (TILE_HEIGHT - 16) / 2;
    display_draw_string(text_from_x, text_from_y, "Text from:",
                        current_theme.text_colour, current_theme.bg_colour, 2);

    // Draw sender number (row 1 of overlay)
    int sender_len = strlen(state->sender_number);
    int sender_width = sender_len * 6 * 2; // 6px per char, size 2
    int sender_x = px + (overlay_px_width - sender_width) / 2;
    int sender_y = py + TILE_HEIGHT + (TILE_HEIGHT - 16) / 2; // Move down one row
    display_draw_string(sender_x, sender_y, state->sender_number,
                        current_theme.text_colour, current_theme.bg_colour, 2);

    // Draw "Open" button (left side)
    tile_to_pixels(1, 5, &px, &py);
    display_draw_string(px + 10, py, "Open",
                        current_theme.fg_colour, current_theme.bg_colour, 1);

    // Draw "Close" button (right side, more to the right)
    tile_to_pixels(5, 5, &px, &py);
    display_draw_string(px + 10, py, "Close",
                        current_theme.fg_colour, current_theme.bg_colour, 1);
}

static void incoming_text_handle_input(Page *self, int event_type)
{
    IncomingTextState *state = (IncomingTextState *)self->state;

    switch (event_type)
    {
    case INPUT_PICKUP:
        // Open text (left side)
        if (state->callback)
        {
            state->callback(INCOMING_TEXT_ACTION_OPEN, state->user_data);
        }
        break;
    case INPUT_HANGUP:
        // Close text (right side)
        if (state->callback)
        {
            state->callback(INCOMING_TEXT_ACTION_CLOSE, state->user_data);
        }
        break;
    default:
        break;
    }
}

static void incoming_text_reset(Page *self)
{
    // No state to reset for draw-based overlay
    mark_all_tiles_dirty();
}

static void incoming_text_destroy(Page *self)
{
    if (!self)
        return;
    IncomingTextState *state = (IncomingTextState *)self->state;
    mem_free(state);
    mem_free(self);
}

Page *incoming_text_overlay_create(const char *sender_number, IncomingTextCallback callback, void *user_data)
{
    if (!sender_number)
        return NULL;

    Page *page = (Page *)mem_malloc(sizeof(Page));
    IncomingTextState *state = (IncomingTextState *)mem_malloc(sizeof(IncomingTextState));

    state->sender_number = sender_number;
    state->callback = callback;
    state->user_data = user_data;

    page->draw = incoming_text_draw;
    page->draw_tile = NULL;
    page->handle_input = incoming_text_handle_input;
    page->reset = incoming_text_reset;
    page->destroy = incoming_text_destroy;
    page->data_response = NULL;
    page->state = state;

    return page;
}
