#include "messages.h"
#include "option_overlay.h"
#include "memwrap.h"

// Helper function to draw wrapped text
static void draw_wrapped_text(int start_x, int start_y, int max_width, const char *text, uint16_t colour, uint16_t bg_colour, uint8_t size)
{
    int x = start_x;
    int y = start_y;
    int line_height = size * 8; // Approximate line height based on font size
    int char_width = size * 5;  // Reduced character width for tighter spacing

    while (*text)
    {
        // Check if current character fits on current line
        if (x + char_width > start_x + max_width && x > start_x)
        {
            // Move to next line
            x = start_x;
            y += line_height;
        }

        // Skip spaces at the beginning of a line
        if (*text == ' ' && x == start_x)
        {
            text++;
            continue;
        }

        // Draw the character
        display_draw_char(x, y, *text, colour, bg_colour, size);
        x += char_width;
        text++;
    }
}

static void messages_page_draw(Page *self)
{
    MessagePageState *state = (MessagePageState *)self->state;
    int px, py;

    // Fill background
    tile_to_pixels(0, 0, &px, &py);
    display_fill_rect(px, py, TILE_COLS * TILE_WIDTH, TILE_ROWS * TILE_HEIGHT, current_theme.bg_colour);

    // Draw sender info
    tile_to_pixels(0, 0, &px, &py);
    display_draw_string(px + 5, py + 10, "From:", current_theme.fg_colour, current_theme.bg_colour, 2);
    display_draw_string(px + 65, py + 10, state->sender, current_theme.text_colour, current_theme.bg_colour, 2);

    // Draw first horizontal separator
    tile_to_pixels(0, 1, &px, &py);
    display_draw_rect(px + 10, py + 5, TILE_COLS * TILE_WIDTH - 20, 1, current_theme.fg_colour);

    // Draw message body with text wrapping
    int message_start_x = px + 15;
    int message_start_y = py + 15;
    int message_width = TILE_COLS * TILE_WIDTH - 30; // Leave margins
    draw_wrapped_text(message_start_x, message_start_y, message_width, state->message, current_theme.text_colour, current_theme.bg_colour, 2);

    // Draw second horizontal separator
    tile_to_pixels(0, 8, &px, &py);
    display_draw_rect(px + 10, py + 5, TILE_COLS * TILE_WIDTH - 20, 1, current_theme.fg_colour);

    draw_bottom_bar("Reply", "", "Back", 0);
}

static void messages_handle_input(Page *self, int event_type)
{
    if (event_type == INPUT_LEFT)
    {
        MessagePageState *state = (MessagePageState *)self->state;
        Page *new_sms_page = new_sms_page_create(state->sender);
        screen_push_page(new_sms_page);
    }
}

static void messages_destroy(Page *self)
{
    if (self == NULL)
        return;
    MessagePageState *state = (MessagePageState *)self->state;
    mem_free(state);
    mem_free(self);
}

Page *messages_page_create(MessagePageState state)
{
    Page *page = mem_malloc(sizeof(Page));
    MessagePageState *page_state = mem_malloc(sizeof(MessagePageState));

    // Copy the passed state data
    strncpy(page_state->sender, state.sender, sizeof(page_state->sender) - 1);
    page_state->sender[sizeof(page_state->sender) - 1] = '\0';
    strncpy(page_state->message, state.message, sizeof(page_state->message) - 1);
    page_state->message[sizeof(page_state->message) - 1] = '\0';

    page->draw = messages_page_draw;
    page->draw_tile = NULL;
    page->handle_input = messages_handle_input;
    page->reset = NULL;
    page->destroy = messages_destroy;
    page->state = page_state;
    return page;
}
