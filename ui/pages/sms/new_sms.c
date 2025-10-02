#include "new_sms.h"
#include "multitap.h"
#include "bottom_bar.h"
#include "option_overlay.h"
#include "memwrap.h"

#define MAX_PHONE_NUMBER_LENGTH 10
#define MAX_SMS_LENGTH 84
#define CHARS_PER_LINE 14
#define CHAR_WIDTH 6
#define CHAR_SCALE 2
#define CHAR_DISPLAY_WIDTH (CHAR_WIDTH * CHAR_SCALE)
#define TEXT_XPAD 5
#define TEXT_YPAD 7

// Option overlay callback
static void test_overlay_callback(int selected_idx, void *user_data)
{
    // For demonstration, just pop the overlay
    screen_pop_page();
    // You can add more logic here based on selected_idx
}

static const char *overlay_options[] = {
    "Send Message",
    "Save to Drafts",
    "Clear"};
#define NUM_OVERLAY_OPTIONS (sizeof(overlay_options) / sizeof(overlay_options[0]))

typedef enum
{
    NUMBER_INPUT,
    SMS_INPUT
} InputMode;

typedef struct
{
    Cursor cursor;
    char phone_number[MAX_PHONE_NUMBER_LENGTH + 1];
    char sms_content[MAX_SMS_LENGTH + 1];
    InputMode mode;
    bool multitap_enabled; // Whether multi-tap is enabled for SMS input
    bool mounted;
    bool overlay_open; // Track if overlay is currently open
} NewSmsState;

static void new_sms_draw(Page *self);
static void new_sms_draw_tile(Page *self, int tx, int ty);
static void new_sms_handle_input(Page *self, int event_type);
static void new_sms_reset(Page *self);
static void new_sms_destroy(Page *self);
static void draw_phone_number_area(Page *self, int tx, int ty);
static void draw_sms_content_area(Page *self, int tx, int ty);
static void add_digit(Page *self, char d);
static void remove_digit(Page *self);
static void add_char(Page *self, char c);
static void remove_char(Page *self);
static void handle_keypad_input(Page *self, int event_type, char digit, char sms_char);
static void calculate_cursor_position(int content_len, int *cursor_x, int *cursor_y);
static void handle_multitap_confirmation(Page *self);
static void update_bottom_bar(NewSmsState *state);

// ==================== Helper Functions ====================

static void calculate_cursor_position(int content_len, int *cursor_x, int *cursor_y)
{
    if (content_len == 0)
    {
        *cursor_x = 0;
        *cursor_y = 0;
    }
    else
    {
        *cursor_y = content_len / CHARS_PER_LINE;
        *cursor_x = content_len % CHARS_PER_LINE;
    }
}

static void handle_keypad_input(Page *self, int event_type, char digit, char sms_char)
{
    NewSmsState *state = (NewSmsState *)self->state;
    if (state->mode == NUMBER_INPUT)
    {
        add_digit(self, digit);
    }
    else
    {
        // For SMS input, use multi-tap if enabled
        if (state->multitap_enabled)
        {
            char output_char;
            if (multitap_handle_keypress(event_type, &output_char))
            {
                add_char(self, output_char);
            }
        }
        else
        {
            add_char(self, sms_char);
        }
    }
}

static void handle_multitap_confirmation(Page *self)
{
    NewSmsState *state = (NewSmsState *)self->state;
    if (state->mode == SMS_INPUT && state->multitap_enabled)
    {
        char output_char;
        if (multitap_confirm_character(&output_char))
        {
            add_char(self, output_char);
        }
    }
}

static void update_bottom_bar(NewSmsState *state)
{
    int accent_index = state->overlay_open ? 1 : 0;
    draw_bottom_bar("Options", "", "Back", accent_index);
}

static void add_digit(Page *self, char d)
{
    NewSmsState *state = (NewSmsState *)self->state;
    if (state->cursor.x < MAX_PHONE_NUMBER_LENGTH)
    {
        state->phone_number[state->cursor.x] = d;
        state->cursor.x++;
        state->phone_number[state->cursor.x] = '\0';
        mark_tile_dirty(1, 1);
    }
}

static void clear_old_digit(NewSmsState *state)
{
    int px, py;
    tile_to_pixels(1, 1, &px, &py);
    int digit_x = px + TILE_WIDTH + (state->cursor.x - 1) * CHAR_DISPLAY_WIDTH;
    display_fill_rect(digit_x, py + TEXT_YPAD, CHAR_DISPLAY_WIDTH, TILE_HEIGHT - TEXT_YPAD, current_theme.fg_colour);
}

static void clear_old_cursor(NewSmsState *state)
{
    int px, py;

    if (state->mode == NUMBER_INPUT)
    {
        tile_to_pixels(1, 1, &px, &py);
        int cursor_x = px + TILE_WIDTH + (state->cursor.x * CHAR_DISPLAY_WIDTH);
        display_fill_rect(cursor_x, py + TEXT_YPAD, 5, TILE_HEIGHT - 8 - TEXT_YPAD, current_theme.fg_colour);
    }
    else if (state->mode == SMS_INPUT)
    {
        tile_to_pixels(1, state->cursor.y + 3, &px, &py);
        int cursor_x = px + TEXT_XPAD + (state->cursor.x * CHAR_DISPLAY_WIDTH);
        int cursor_y = py + TEXT_YPAD;
        display_fill_rect(cursor_x, cursor_y, 5, TILE_HEIGHT - 8 - TEXT_YPAD, current_theme.bg_colour);
    }
}

static void remove_digit(Page *self)
{
    NewSmsState *state = (NewSmsState *)self->state;
    if (state->cursor.x > 0)
    {
        clear_old_digit(state);
        clear_old_cursor(state);
        state->cursor.x--;
        state->phone_number[state->cursor.x] = '\0';
        mark_tile_dirty(1, 1);
    }
}

static void draw_number(NewSmsState *state, int tx, int ty)
{
    int px, py;
    int xpad = TILE_WIDTH;
    tile_to_pixels(1, 1, &px, &py);
    display_draw_string(px + xpad, py + TEXT_YPAD, state->phone_number, current_theme.text_colour, current_theme.fg_colour, CHAR_SCALE);
    int cursor_x = px + xpad + (strlen(state->phone_number) * CHAR_DISPLAY_WIDTH);
    display_fill_rect(cursor_x, py + TEXT_YPAD, 5, TILE_HEIGHT - 8 - TEXT_YPAD, current_theme.text_colour);
}

static void draw_phone_number_area(Page *self, int tx, int ty)
{
    NewSmsState *state = (NewSmsState *)self->state;
    int px, py;
    tile_to_pixels(tx, ty, &px, &py);
    int width = TILE_WIDTH * (TILE_COLS - 2);
    int height = TILE_HEIGHT;
    if (tx == 0)
    {
        display_fill_rect(px, py, TILE_COLS * TILE_WIDTH, TILE_ROWS * TILE_HEIGHT, current_theme.bg_colour);
        display_fill_rect(TILE_WIDTH, py + TILE_HEIGHT, width, height, current_theme.fg_colour);
        const char *label = "   To:";
        display_draw_string(px + 5, py + 10, label, current_theme.fg_colour, current_theme.bg_colour, 2);
    }
    draw_number(state, tx, ty);
    for (int i = 0; i < TILE_COLS; i++)
    {
        mark_tile_clean(i, 0);
        mark_tile_clean(i, 1);
    }
}

static void add_char(Page *self, char c)
{
    NewSmsState *state = (NewSmsState *)self->state;
    if (strlen(state->sms_content) < MAX_SMS_LENGTH)
    {
        // Add character to string content
        mark_tile_dirty(1, state->cursor.y + 3);
        int content_len = strlen(state->sms_content);
        state->sms_content[content_len] = c;
        state->sms_content[content_len + 1] = '\0';
        content_len++; // Update length after adding character

        calculate_cursor_position(content_len, &state->cursor.x, &state->cursor.y);
        mark_tile_dirty(1, state->cursor.y + 3);
    }
}

static void clear_old_char(NewSmsState *state)
{
    // Calculate the position of the character that was just removed
    int content_len = strlen(state->sms_content);
    if (content_len > 0)
    {
        // Calculate where the last character was positioned
        int last_char_line = (content_len - 1) / CHARS_PER_LINE;
        int last_char_pos = (content_len - 1) % CHARS_PER_LINE;

        // Use tile-based positioning like the drawing functions
        int px, py;
        tile_to_pixels(1, last_char_line + 3, &px, &py);
        int char_x = px + TEXT_XPAD + (last_char_pos * CHAR_DISPLAY_WIDTH);
        int char_y = py + TEXT_YPAD;

        // Clear the character area
        display_fill_rect(char_x, char_y, CHAR_DISPLAY_WIDTH, 16, current_theme.bg_colour);
    }
}

static void remove_char(Page *self)
{
    NewSmsState *state = (NewSmsState *)self->state;
    int content_len = strlen(state->sms_content);
    if (content_len > 0)
    {
        // Clear old cursor and character before making changes
        clear_old_cursor(state);
        clear_old_char(state);

        // Remove last character from string
        state->sms_content[content_len - 1] = '\0';

        // Recalculate cursor position based on new text length
        content_len--;
        int old_line = state->cursor.y;
        calculate_cursor_position(content_len, &state->cursor.x, &state->cursor.y);

        // Mark the line where character was removed
        mark_tile_dirty(1, old_line + 3);
        // If cursor moved to different line, mark that too
        if (state->cursor.y != old_line)
        {
            mark_tile_dirty(1, state->cursor.y + 3);
        }
    }
}

static void draw_sms_content_area(Page *self, int tx, int ty)
{
    NewSmsState *state = (NewSmsState *)self->state;
    int px, py;
    tile_to_pixels(tx, ty, &px, &py);
    int width = TILE_WIDTH * (TILE_COLS - 2);
    int height = TILE_HEIGHT * 6 - 1;
    if (tx == 0 && ty == 2)
    {
        display_draw_rect(TILE_WIDTH, py + TILE_HEIGHT, width, height, current_theme.fg_colour);
        for (int i = 2; i < 8; i++)
        {
            display_draw_horizontal_line(TILE_WIDTH + 1, (i * TILE_HEIGHT) + py, TILE_WIDTH + width - 2, current_theme.highlight_colour);
        }
    }
    // Removed unused variables xpad and ypad

    if (tx == 1)
    {
        int line = ty - 3;
        char line_buffer[15];
        int p = 0;
        int start_pos = line * CHARS_PER_LINE;

        // Extract characters for this line
        while (p < CHARS_PER_LINE && start_pos + p < strlen(state->sms_content))
        {
            line_buffer[p] = state->sms_content[start_pos + p];
            p++;
        }
        line_buffer[p] = '\0'; // Null terminate

        // Only draw if there's content on this line
        if (p > 0)
        {
            display_draw_string(px + TEXT_XPAD, py + TEXT_YPAD, line_buffer, current_theme.text_colour, current_theme.bg_colour, CHAR_SCALE);
        }
        if (state->mode == SMS_INPUT && line == state->cursor.y)
        {
            int cursor_x = px + TEXT_XPAD + (state->cursor.x * CHAR_DISPLAY_WIDTH);
            int cursor_y = py + TEXT_YPAD;

            // Draw cursor
            display_fill_rect(cursor_x, cursor_y, 5, TILE_HEIGHT - 8 - TEXT_YPAD, current_theme.text_colour);

            // Draw multi-tap preview if active
            if (state->multitap_enabled && multitap_is_active())
            {
                char preview_char = multitap_get_current_char();
                if (preview_char != '\0')
                {
                    char preview_str[2] = {preview_char, '\0'};
                    // Draw preview character in a different color (e.g., dimmed)
                    display_draw_string(cursor_x + 8, cursor_y, preview_str, 0x7FFF, current_theme.bg_colour, CHAR_SCALE);
                }
            }
        }
    }
}

// ==================== VTABLE Functions ====================

static void new_sms_draw_tile(Page *self, int tx, int ty)
{
    NewSmsState *state = (NewSmsState *)self->state;
    int row = ty / 2;
    if (row == 0)
    {
        draw_phone_number_area(self, tx, ty);
    }
    else if (row >= 1)
    {
        draw_sms_content_area(self, tx, ty);
    }
    for (int i = 0; i < TILE_COLS; i++)
    {
        mark_tile_clean(i, row * 2);
    }
    if (!state->mounted)
    {
        update_bottom_bar(state);
        state->mounted = true;
    }
}

static void new_sms_draw(Page *self) {}

static void new_sms_handle_input(Page *self, int event_type)
{
    NewSmsState *state = (NewSmsState *)self->state;

    // Check if overlay was closed (any input other than overlay opening means overlay is closed)
    if (state->overlay_open && event_type != INPUT_LEFT)
    {
        state->overlay_open = false;
        update_bottom_bar(state);
    }
    switch (event_type)
    {
    case INPUT_KEYPAD_0:
        handle_keypad_input(self, event_type, '0', ' ');
        break;
    case INPUT_KEYPAD_1:
        handle_keypad_input(self, event_type, '1', '.');
        break;
    case INPUT_KEYPAD_2:
        handle_keypad_input(self, event_type, '2', 'a');
        break;
    case INPUT_KEYPAD_3:
        handle_keypad_input(self, event_type, '3', 'd');
        break;
    case INPUT_KEYPAD_4:
        handle_keypad_input(self, event_type, '4', 'g');
        break;
    case INPUT_KEYPAD_5:
        handle_keypad_input(self, event_type, '5', 'j');
        break;
    case INPUT_KEYPAD_6:
        handle_keypad_input(self, event_type, '6', 'm');
        break;
    case INPUT_KEYPAD_7:
        handle_keypad_input(self, event_type, '7', 'p');
        break;
    case INPUT_KEYPAD_8:
        handle_keypad_input(self, event_type, '8', 't');
        break;
    case INPUT_KEYPAD_9:
        handle_keypad_input(self, event_type, '9', 'w');
        break;
    case INPUT_KEYPAD_STAR:
        // Toggle multi-tap mode
        if (state->mode == SMS_INPUT)
        {
            state->multitap_enabled = !state->multitap_enabled;
            multitap_reset(); // Reset any pending multi-tap
        }
        break;
    case INPUT_KEYPAD_HASH:
        // Confirm current multi-tap character if active
        if (state->mode == SMS_INPUT && state->multitap_enabled)
        {
            handle_multitap_confirmation(self);
        }
        break;
    case INPUT_DPAD_LEFT:
        if (state->mode == NUMBER_INPUT)
            remove_digit(self);
        else
            remove_char(self);
        break;
    case INPUT_DPAD_RIGHT:
        break; // Backspace for phone number
    case INPUT_DPAD_UP:
        if (state->mode == SMS_INPUT)
        {
            clear_old_cursor(state);
            state->mode = NUMBER_INPUT;
            state->cursor.x = strlen(state->phone_number);
            state->cursor.y = 0;
            mark_tile_dirty(1, 1);
        }
        break;
    case INPUT_DPAD_DOWN:
        if (state->mode == NUMBER_INPUT)
        {
            clear_old_cursor(state);
            state->mode = SMS_INPUT;
            int content_len = strlen(state->sms_content);
            calculate_cursor_position(content_len, &state->cursor.x, &state->cursor.y);
            // Mark only the line where cursor is now positioned
            mark_tile_dirty(1, state->cursor.y + 3);
        }
        break;
    case INPUT_LEFT:
        // Show option overlay
        state->overlay_open = true;
        update_bottom_bar(state);
        Page *overlay = option_overlay_page_create(
            "Message Options",
            overlay_options,
            NUM_OVERLAY_OPTIONS,
            test_overlay_callback,
            NULL);
        if (overlay)
            screen_push_page(overlay);
    case INPUT_SELECT:
        break;
    default:
        break;
    }
}

static void new_sms_reset(Page *self)
{
    NewSmsState *state = (NewSmsState *)self->state;
    cursor_reset(&state->cursor);
    memset(state->phone_number, 0, sizeof(state->phone_number));
    memset(state->sms_content, 0, sizeof(state->sms_content));
    state->mode = NUMBER_INPUT;
    state->multitap_enabled = true; // Enable multi-tap by default
    multitap_reset();
    state->mounted = false;
    state->overlay_open = false;
}
static void new_sms_destroy(Page *self)
{
    if (self)
    {
        NewSmsState *state = (NewSmsState *)self->state;
        mem_free(state);
        mem_free(self);
    }
}

Page *new_sms_page_create()
{
    Page *page = mem_malloc(sizeof(Page));
    NewSmsState *state = mem_malloc(sizeof(NewSmsState));

    state->cursor = (Cursor){0, 0, 0, MAX_PHONE_NUMBER_LENGTH - 1, false};
    memset(state->phone_number, 0, sizeof(state->phone_number));
    memset(state->sms_content, 0, sizeof(state->sms_content));
    state->mode = NUMBER_INPUT;
    state->multitap_enabled = true; // Enable multi-tap by default
    state->mounted = false;
    state->overlay_open = false;

    page->draw = new_sms_draw;
    page->draw_tile = new_sms_draw_tile;
    page->handle_input = new_sms_handle_input;
    page->reset = new_sms_reset;
    page->destroy = new_sms_destroy;
    page->state = state;

    // Initialize multi-tap system
    multitap_init();

    return page;
}