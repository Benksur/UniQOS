#include "new_sms.h"

#define MAX_PHONE_NUMBER_LENGTH 10
#define MAX_SMS_LENGTH 140

typedef enum {
    NUMBER_INPUT,
    SMS_INPUT
} InputMode;

typedef struct {
    Cursor cursor;
    char phone_number[MAX_PHONE_NUMBER_LENGTH + 1];
    char sms_content[MAX_SMS_LENGTH + 1];
    InputMode mode;
} NewSmsState;

static void new_sms_draw(Page* self);
static void new_sms_draw_tile(Page* self, int tx, int ty);
static void new_sms_handle_input(Page* self, int event_type);
static void new_sms_reset(Page* self);
static void new_sms_destroy(Page* self);
static void draw_phone_number_area(Page* self, int tx, int ty);
static void draw_sms_content_area(Page* self, int tx, int ty);
static void add_digit(Page* self, char d);
static void remove_digit(Page* self);
static void add_char(Page* self, char c);
static void remove_char(Page* self);

// ==================== Helper Functions ====================

static void add_digit(Page* self, char d) {
    NewSmsState* state = (NewSmsState*)self->state;
    if (state->cursor.x < MAX_PHONE_NUMBER_LENGTH) {
        state->phone_number[state->cursor.x] = d;
        state->cursor.x++;
        state->phone_number[state->cursor.x] = '\0';
        
        // Mark phone number area dirty for redraw
        mark_tile_dirty(1, 1);
    }
}

static void clear_old_digit(NewSmsState* state) {
    int px, py;
    tile_to_pixels(1, 1, &px, &py);
    int digit_x = px + TILE_WIDTH + (state->cursor.x - 1) * 6 * 2;
    display_fill_rect(digit_x, py + 7, 6 * 2, TILE_HEIGHT - 7, current_theme.fg_colour);
}

static void clear_old_cursor(NewSmsState* state) {
    int px, py;
    int xpad = 5, ypad = 7;
    
    if (state->mode == NUMBER_INPUT) {
        tile_to_pixels(1, 1, &px, &py);
        int cursor_x = px + TILE_WIDTH + (state->cursor.x * 6 * 2); 
        display_fill_rect(cursor_x, py + ypad, 5, TILE_HEIGHT - 8 - 7, current_theme.fg_colour);
    } else if (state->mode == SMS_INPUT) {
        // Calculate the absolute position for multi-line SMS cursor
        int cursor_x = TILE_WIDTH + xpad + (state->cursor.x * 6 * 2);
        int cursor_y = NAVBAR_HEIGHT + (3 * TILE_HEIGHT) + ypad + (state->cursor.y * 16);
        display_fill_rect(cursor_x, cursor_y, 5, TILE_HEIGHT - 8 - ypad, current_theme.bg_colour);
    }
}

static void remove_digit(Page* self) {
    NewSmsState* state = (NewSmsState*)self->state;
    if (state->cursor.x > 0) {
        clear_old_digit(state);
        clear_old_cursor(state);
        state->cursor.x--;
        state->phone_number[state->cursor.x] = '\0';
        
        // Mark phone number area dirty for redraw
        mark_tile_dirty(1, 1);
    }
}

static void draw_number(NewSmsState* state, int tx, int ty) {
    int px, py;
    int xpad = TILE_WIDTH, ypad = 7;
    tile_to_pixels(1, 1, &px, &py);
    display_draw_string(px + xpad, py + ypad, state->phone_number, current_theme.text_colour, current_theme.fg_colour, 2);
    int cursor_x = px + xpad + (strlen(state->phone_number) * 6 * 2); 
    display_fill_rect(cursor_x, py + ypad, 5, TILE_HEIGHT - 8 - ypad, current_theme.text_colour);
}

static void draw_phone_number_area(Page* self, int tx, int ty) {
    NewSmsState* state = (NewSmsState*)self->state;
    int px, py;
    tile_to_pixels(tx, ty, &px, &py);
    int width = TILE_WIDTH * (TILE_COLS - 2);
    int height = TILE_HEIGHT;
    if (tx == 0) {
        display_fill_rect(px, py, TILE_COLS * TILE_WIDTH, TILE_ROWS * TILE_HEIGHT, current_theme.bg_colour);
        display_fill_rect(TILE_WIDTH, py + TILE_HEIGHT, width, height, current_theme.fg_colour);
        const char* label = "   To:";
        display_draw_string(px + 5, py + 10, label, current_theme.fg_colour, current_theme.bg_colour, 2);
    }
    draw_number(state, tx, ty);
    for (int i = 0; i < TILE_COLS; i++) {mark_tile_clean(i, 0); mark_tile_clean(i, 1);}
}

static void add_char(Page* self, char c) {
    NewSmsState* state = (NewSmsState*)self->state;
    if (strlen(state->sms_content) < MAX_SMS_LENGTH) {
        // Add character to string content
        int content_len = strlen(state->sms_content);
        state->sms_content[content_len] = c;
        state->sms_content[content_len + 1] = '\0';
        content_len++; // Update length after adding character
        
        // Calculate cursor position based on text length and line wrapping
        state->cursor.y = content_len / 14;  // 14 chars per line (0-13)
        state->cursor.x = content_len % 14;
        
        // Mark SMS content area dirty for redraw
        mark_tile_dirty(1, 3);
    }
}

static void clear_old_char(NewSmsState* state) {
    // Calculate the position of the character that was just removed
    int content_len = strlen(state->sms_content);
    if (content_len >= 0) {
        // Calculate where the last character was positioned
        int last_char_line = content_len / 14;
        int last_char_pos = content_len % 14;
        
        int xpad = 5, ypad = 7;
        int char_x = TILE_WIDTH + xpad + (last_char_pos * 6 * 2);
        int char_y = NAVBAR_HEIGHT + (3 * TILE_HEIGHT) + ypad + (last_char_line * 16);
        
        // Clear the character area
        display_fill_rect(char_x, char_y, 6 * 2, 16, current_theme.bg_colour);
    }
}

static void remove_char(Page* self) {
    NewSmsState* state = (NewSmsState*)self->state;
    int content_len = strlen(state->sms_content);
    if (content_len > 0) {
        // Clear old cursor and character before making changes
        clear_old_cursor(state);
        clear_old_char(state);
        
        // Remove last character from string
        state->sms_content[content_len - 1] = '\0';
        
        // Recalculate cursor position based on new text length
        content_len--;
        if (content_len == 0) {
            state->cursor.x = 0;
            state->cursor.y = 0;
        } else {
            state->cursor.y = content_len / 14;  // 14 chars per line (0-13)
            state->cursor.x = content_len % 14;
        }
        
        // Mark SMS content area dirty for redraw
        mark_tile_dirty(1, 3);
    }
}

static void draw_sms_content_area(Page* self, int tx, int ty) {
    NewSmsState* state = (NewSmsState*)self->state;
    int px, py;
    tile_to_pixels(tx, ty, &px, &py);
    int width = TILE_WIDTH * (TILE_COLS - 2);
    int height = TILE_HEIGHT * 6;
    if (tx == 0) {
        display_draw_rect(TILE_WIDTH, py + TILE_HEIGHT, width, height, current_theme.fg_colour);
        for (int i = 2; i < 8; i++) {
            display_draw_horizontal_line(TILE_WIDTH + 1, (i * TILE_HEIGHT) + py, TILE_WIDTH + width - 2, current_theme.highlight_colour);
        }
    }
    int xpad = 5, ypad = 7;
    
    // Draw text line by line for proper wrapping
    char* content = state->sms_content;
    int content_len = strlen(content);
    int line = 0;
    
    for (int i = 0; i < content_len; i += 14) {
        char line_buffer[15];
        int chars_in_line = (content_len - i < 14) ? content_len - i : 14;
        strncpy(line_buffer, content + i, chars_in_line);
        line_buffer[chars_in_line] = '\0';
        
        int line_y = py + ypad + (line * 16); // 16 pixels between lines
        display_draw_string(px + xpad, line_y, line_buffer, current_theme.text_colour, current_theme.bg_colour, 2);
        line++;
    }
    
    // Draw cursor if in SMS input mode
    if (state->mode == SMS_INPUT) {
        int cursor_x = px + xpad + (state->cursor.x * 6 * 2);
        int cursor_y = py + ypad + (state->cursor.y * 16);
        display_fill_rect(cursor_x, cursor_y, 5, TILE_HEIGHT - 8 - ypad, current_theme.text_colour);
    }
}

// ==================== VTABLE Functions ====================

static void new_sms_draw_tile(Page* self, int tx, int ty) {
    NewSmsState* state = (NewSmsState*)self->state;
    int row = ty / 2;
    if (row == 0) {
        draw_phone_number_area(self, tx, ty);
    } 
    else if (row == 1) draw_sms_content_area(self, tx, ty);
    for (int i = 0; i < TILE_COLS; i++) {
        mark_tile_clean(i, row * 2);
        mark_tile_clean(i, row * 2 + 1);
    }
    
}

static void new_sms_draw(Page* self) {}
static void new_sms_handle_input(Page* self, int event_type) {
    NewSmsState* state = (NewSmsState*)self->state;
    switch (event_type) {
        case INPUT_KEYPAD_0: 
            if (state->mode == NUMBER_INPUT){
                add_digit(self, '0'); 
            } else {
                add_char(self, ' ');
            }
            break;
        case INPUT_KEYPAD_1: 
            if (state->mode == NUMBER_INPUT) {
                add_digit(self, '1'); 
            } else {
                add_char(self, '.');
            }
            break;
        case INPUT_KEYPAD_2: 
            if (state->mode == NUMBER_INPUT) {
                add_digit(self, '2'); 
            } else {
                add_char(self, 'a');
            }
            break;
        case INPUT_KEYPAD_3: 
            if (state->mode == NUMBER_INPUT) {
                add_digit(self, '3'); 
            } else {
                add_char(self, 'd');
            }
            break;
        case INPUT_KEYPAD_4: 
            if (state->mode == NUMBER_INPUT) {
                add_digit(self, '4'); 
            } else {
                add_char(self, 'g');
            }
            break;
        case INPUT_KEYPAD_5: 
            if (state->mode == NUMBER_INPUT) {
                add_digit(self, '5'); 
            } else {
                add_char(self, 'j');
            }
            break;
        case INPUT_KEYPAD_6: 
            if (state->mode == NUMBER_INPUT) {
                add_digit(self, '6'); 
            } else {
                add_char(self, 'm');
            }
            break;
        case INPUT_KEYPAD_7: 
            if (state->mode == NUMBER_INPUT) {
                add_digit(self, '7'); 
            } else {
                add_char(self, 'p');
            }
            break;
        case INPUT_KEYPAD_8: 
            if (state->mode == NUMBER_INPUT) {
                add_digit(self, '8'); 
            } else {
                add_char(self, 't');
            }
            break;
        case INPUT_KEYPAD_9: 
            if (state->mode == NUMBER_INPUT) {
                add_digit(self, '9'); 
            } else {
                add_char(self, 'w');
            }
            break;
        case INPUT_KEYPAD_STAR: break;
        case INPUT_KEYPAD_HASH: break;
        case INPUT_DPAD_LEFT:
            if (state->mode == NUMBER_INPUT) remove_digit(self); 
            else remove_char(self); 
            break;
        case INPUT_DPAD_RIGHT:  break; // Backspace for phone number
        case INPUT_DPAD_UP: 
            if (state->mode == SMS_INPUT) {
                clear_old_cursor(state);
                state->mode = NUMBER_INPUT;
                state->cursor.x = strlen(state->phone_number);
                state->cursor.y = 0;
                mark_tile_dirty(1, 1);
            }
            break;
        case INPUT_DPAD_DOWN: 
            if (state->mode == NUMBER_INPUT) {
                clear_old_cursor(state);
                state->mode = SMS_INPUT;
                int content_len = strlen(state->sms_content);
                if (content_len == 0) {
                    state->cursor.x = 0;
                    state->cursor.y = 0;
                } else {
                    state->cursor.y = content_len / 14;
                    state->cursor.x = content_len % 14;
                }
                mark_tile_dirty(1, 3);
            }
            break;
        case INPUT_SELECT: break; // TODO: Send SMS
        case INPUT_LEFT: screen_pop_page(); break;
        default: break;
    }
}

static void new_sms_reset(Page* self) {}
static void new_sms_destroy(Page* self) {}

Page* new_sms_page_create() {
    Page* page = malloc(sizeof(Page));
    NewSmsState* state = malloc(sizeof(NewSmsState));
    
    // Initialize cursor (x position tracks digit position)
    state->cursor = (Cursor){0, 0, 0, MAX_PHONE_NUMBER_LENGTH - 1, false};
    
    // Initialize phone number and SMS content
    memset(state->phone_number, 0, sizeof(state->phone_number));
    memset(state->sms_content, 0, sizeof(state->sms_content));
    state->mode = NUMBER_INPUT;
    
    page->draw = new_sms_draw;
    page->draw_tile = new_sms_draw_tile;
    page->handle_input = new_sms_handle_input;
    page->reset = new_sms_reset;
    page->destroy = new_sms_destroy;
    page->state = state;
    
    return page;
}