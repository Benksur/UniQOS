#include "calculator.h"
#include "screen.h"
#include "display.h"
#include "tile.h"
#include "input.h"
#include "theme.h"
#include "cursor.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define MAX_DISPLAY_LENGTH 13
#define CALC_DISPLAY_WIDTH (TILE_WIDTH * (TILE_COLS - 2))
#define CALC_DISPLAY_HEIGHT TILE_HEIGHT

typedef enum {
    OP_NONE,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV
} Operation;

typedef struct {
    char display[MAX_DISPLAY_LENGTH + 1];
    double operand1;
    double operand2;
    Operation operation;
    bool new_number;
    bool error;
    Cursor cursor;
} CalculatorState;

static void calculator_draw(Page *self);
static void calculator_draw_tile(Page *self, int tx, int ty);
static void calculator_handle_input(Page *self, int event_type);
static void calculator_destroy(Page *self);
static void draw_display_area(Page *self, int tx, int ty);
static void draw_button_area(Page *self, int tx, int ty);
static void draw_large_button_tile(int tx, int ty, int width_tiles, int height_tiles, const char* label, bool selected);
static void add_digit(CalculatorState *state, char digit);
static void add_decimal(CalculatorState *state);
static void handle_keypad_input(Page *self, int event_type, char digit);
static void handle_operator(CalculatorState *state, char op);

// ========================= HELPERS ================================== //

static void clear_calculator(CalculatorState *state) {
    strcpy(state->display, "0");
    state->operand1 = 0;
    state->operand2 = 0;
    state->operation = OP_NONE;
    state->new_number = true;
    state->error = false;
}

static void update_display_number(CalculatorState *state, double number) {
    if (number == (int)number && abs((int)number) < 1000000) {
        sprintf(state->display, "%d", (int)number);
    } else {
        sprintf(state->display, "%.6g", number);
    }
    
    // Truncate if too long
    if (strlen(state->display) > MAX_DISPLAY_LENGTH) {
        strcpy(state->display, "Error");
        state->error = true;
    }
}

static double get_display_value(CalculatorState *state) {
    return atof(state->display);
}

static void add_digit(CalculatorState *state, char digit) {
    if (state->error) return;
    
    if (state->new_number) {
        if (digit == '0') {
            strcpy(state->display, "0");
        } else {
            state->display[0] = digit;
            state->display[1] = '\0';
        }
        state->new_number = false;
    } else {
        if (strcmp(state->display, "0") == 0 && digit != '.') {
            state->display[0] = digit;
            state->display[1] = '\0';
        } else {
            int len = strlen(state->display);
            if (len < MAX_DISPLAY_LENGTH) {
                state->display[len] = digit;
                state->display[len + 1] = '\0';
            }
        }
    }
    mark_tile_dirty(1, 1); // Mark display tile as dirty
}

static void add_decimal(CalculatorState *state) {
    if (state->error) return;
    
    if (state->new_number) {
        strcpy(state->display, "0.");
        state->new_number = false;
    } else {
        // Check if decimal already exists
        if (strchr(state->display, '.') == NULL) {
            int len = strlen(state->display);
            if (len < MAX_DISPLAY_LENGTH) {
                state->display[len] = '.';
                state->display[len + 1] = '\0';
            }
        }
    }
    mark_tile_dirty(1, 1); // Mark display tile as dirty
}

static void handle_keypad_input(Page *self, int event_type, char digit) {
    CalculatorState *state = (CalculatorState *)self->state;
    add_digit(state, digit);
}

static void set_operation(CalculatorState *state, Operation op) {
    if (state->error) return;
    
    if (state->operation != OP_NONE && !state->new_number) {
        // Perform pending operation first
        state->operand2 = get_display_value(state);
        double result = 0;
        
        switch (state->operation) {
            case OP_ADD: result = state->operand1 + state->operand2; break;
            case OP_SUB: result = state->operand1 - state->operand2; break;
            case OP_MUL: result = state->operand1 * state->operand2; break;
            case OP_DIV:
                if (state->operand2 == 0) {
                    strcpy(state->display, "Error");
                    state->error = true;
                    return;
                }
                result = state->operand1 / state->operand2;
                break;
            default: result = state->operand2; break;
        }
        
        update_display_number(state, result);
        state->operand1 = result;
    } else {
        state->operand1 = get_display_value(state);
    }
    
    state->operation = op;
    state->new_number = true;
    mark_tile_dirty(1, 1); // Mark display tile as dirty
}

static void calculate_result(CalculatorState *state) {
    if (state->error || state->operation == OP_NONE) return;
    
    state->operand2 = get_display_value(state);
    double result = 0;
    
    switch (state->operation) {
        case OP_ADD: result = state->operand1 + state->operand2; break;
        case OP_SUB: result = state->operand1 - state->operand2; break;
        case OP_MUL: result = state->operand1 * state->operand2; break;
        case OP_DIV:
            if (state->operand2 == 0) {
                strcpy(state->display, "Error");
                state->error = true;
                return;
            }
            result = state->operand1 / state->operand2;
            break;
        default: return;
    }
    
    update_display_number(state, result);
    state->operation = OP_NONE;
    state->new_number = true;
    mark_tile_dirty(1, 1); // Mark display tile as dirty
}

static void handle_operator(CalculatorState *state, char op) {
    switch (op) {
        case '+': set_operation(state, OP_ADD); break;
        case '-': set_operation(state, OP_SUB); break;
        case '*': set_operation(state, OP_MUL); break;
        case '/': set_operation(state, OP_DIV); break;
        case '=': calculate_result(state); break;
        case 'C': clear_calculator(state); mark_tile_dirty(1, 1); break;
    }
}

// ========================= DRAWING ================================== //

static void draw_display_area(Page *self, int tx, int ty) {
    CalculatorState *state = (CalculatorState *)self->state;
    int px, py;
    tile_to_pixels(tx, ty, &px, &py);
    
    if (tx == 0 && ty == 0) {
        // Clear background and draw title
        display_fill_rect(px, py, TILE_COLS * TILE_WIDTH, TILE_HEIGHT * (TILE_ROWS + 1), current_theme.bg_colour);
        display_fill_rect(px + TILE_WIDTH, py + TILE_WIDTH, CALC_DISPLAY_WIDTH, CALC_DISPLAY_HEIGHT, current_theme.highlight_colour);
        display_draw_rect(px + TILE_WIDTH, py + TILE_WIDTH, CALC_DISPLAY_WIDTH, CALC_DISPLAY_HEIGHT, current_theme.fg_colour);
    }
    
    if (tx == 1 && ty == 1) {

        // Draw the number/result (right aligned)
        int text_size = 2;
        int text_width = strlen(state->display) * 6 * text_size;
        int text_x = px + CALC_DISPLAY_WIDTH - text_width - 10;
        int text_y = py + (CALC_DISPLAY_HEIGHT - 8 * text_size) / 2;
        
        uint16_t text_color = state->error ? COLOUR_RED : current_theme.fg_colour;
        display_fill_rect(px + 1, py + 1, CALC_DISPLAY_WIDTH - 2, CALC_DISPLAY_HEIGHT - 2, current_theme.highlight_colour);
        display_draw_string(text_x, text_y, state->display, text_color, current_theme.highlight_colour, text_size);
    }
}

static void draw_button_tile(int tx, int ty, const char* label, bool selected) {
    int px, py;
    tile_to_pixels(tx, ty, &px, &py);
    
    // Choose colors based on button type
    uint16_t bg_color, text_color;
    if (strcmp(label, "C") == 0) {
        bg_color = COLOUR_RED;
        text_color = COLOUR_WHITE;
    } else if (strcmp(label, "=") == 0) {
        bg_color = current_theme.bg_colour;
        text_color = current_theme.text_colour;
    } else {
        bg_color = current_theme.bg_colour;
        text_color = current_theme.text_colour;
    }
    
    // Draw button background
    display_fill_rect(px, py, TILE_WIDTH, TILE_HEIGHT, bg_color);
    
    // Draw outline - white if selected
    uint16_t outline_color = selected ? COLOUR_WHITE : current_theme.fg_colour;
    display_draw_rect(px, py, TILE_WIDTH, TILE_HEIGHT, outline_color);
    
    // Draw button text centered
    int text_width = strlen(label) * 6 * 2;
    int text_x = px + (TILE_WIDTH - text_width) / 2;
    int text_y = py + (TILE_HEIGHT - 16) / 2;
    
    display_draw_string(text_x, text_y, label, text_color, bg_color, 2);
}

static void draw_large_button_tile(int tx, int ty, int width_tiles, int height_tiles, const char* label, bool selected) {
    int px, py;
    tile_to_pixels(tx, ty, &px, &py);
    
    int button_width = width_tiles * TILE_WIDTH;
    int button_height = height_tiles * TILE_HEIGHT;
    
    // Choose colors based on button type
    uint16_t bg_color, text_color;
    if (strcmp(label, "C") == 0) {
        bg_color = current_theme.fg_colour;
        text_color = current_theme.text_colour;
    } else if (strcmp(label, "=") == 0) {
        bg_color = current_theme.accent_colour;
        text_color = current_theme.bg_colour;
    } else {
        bg_color = current_theme.bg_colour;
        text_color = current_theme.text_colour;
    }
    
    // Draw button background
    display_fill_rect(px, py, button_width, button_height, bg_color);
    
    // Draw outline - white if selected
    uint16_t outline_color = selected ? COLOUR_WHITE : current_theme.fg_colour;
    display_draw_rect(px, py, button_width, button_height, outline_color);
    
    // Draw button text centered
    int text_size = 3; // Larger text for larger buttons
    int text_width = strlen(label) * 6 * text_size;
    int text_x = px + (button_width - text_width) / 2;
    int text_y = py + (button_height - 8 * text_size) / 2;
    
    display_draw_string(text_x, text_y, label, text_color, bg_color, text_size);
}

static void draw_button_area(Page *self, int tx, int ty) {
    CalculatorState *state = (CalculatorState *)self->state;
    
    // Button layout: 3x2 grid with 2x2 tile buttons
    // Row 3-4: C     /     *
    // Row 5-6: +     -     =     
    const char* button_layout[2][3] = {
        {"C", "/", "*"},
        {"+", "-", "="}
    };
    
    if (ty >= 3 && ty <= 6) {
        // Determine which button this tile belongs to
        int button_row = (ty - 3) / 2;
        int button_col = (tx - 1) / 2;
        
        if (tx >= 1 && tx <= 6 && button_row < 2 && button_col < 3) {
            const char* label = button_layout[button_row][button_col];
            
            // Only draw on top-left tile of each 2x2 button
            bool is_top_left = ((ty - 3) % 2 == 0) && ((tx - 1) % 2 == 0);
            if (!is_top_left) return;
            
            bool selected = (state->cursor.x == button_col && state->cursor.y == button_row);
            
            draw_large_button_tile(tx, ty, 2, 2, label, selected);
        }
    }
}

// ========================= VTABLE FUNCTIONS ========================= //

static void calculator_draw_tile(Page *self, int tx, int ty) {
    if (ty <= 1) {
        draw_display_area(self, tx, ty);
    } else if (ty >= 3 && ty <= 6) {
        draw_button_area(self, tx, ty);
    }
    mark_tile_clean(tx, ty);
}

static void calculator_draw(Page *self) {
    // display_fill_rect(0, 30, TILE_WIDTH * TILE_COLS, TILE_HEIGHT * (TILE_ROWS + 1), current_theme.bg_colour);
}

static void calculator_handle_input(Page *self, int event_type) {
    CalculatorState *state = (CalculatorState *)self->state;
    
    switch (event_type) {
        // Handle keypad input directly (numbers)
        case INPUT_KEYPAD_0: handle_keypad_input(self, event_type, '0'); break;
        case INPUT_KEYPAD_1: handle_keypad_input(self, event_type, '1'); break;
        case INPUT_KEYPAD_2: handle_keypad_input(self, event_type, '2'); break;
        case INPUT_KEYPAD_3: handle_keypad_input(self, event_type, '3'); break;
        case INPUT_KEYPAD_4: handle_keypad_input(self, event_type, '4'); break;
        case INPUT_KEYPAD_5: handle_keypad_input(self, event_type, '5'); break;
        case INPUT_KEYPAD_6: handle_keypad_input(self, event_type, '6'); break;
        case INPUT_KEYPAD_7: handle_keypad_input(self, event_type, '7'); break;
        case INPUT_KEYPAD_8: handle_keypad_input(self, event_type, '8'); break;
        case INPUT_KEYPAD_9: handle_keypad_input(self, event_type, '9'); break;
        case INPUT_KEYPAD_STAR: add_decimal(state); break; // * key for decimal
        
        // Handle D-pad navigation for operators
        case INPUT_DPAD_UP:
            if (state->cursor.y > 0) {
                mark_tile_dirty(state->cursor.x * 2 + 1, state->cursor.y * 2 + 3);
                mark_tile_dirty(state->cursor.x * 2 + 2, state->cursor.y * 2 + 3);
                mark_tile_dirty(state->cursor.x * 2 + 1, state->cursor.y * 2 + 4);
                mark_tile_dirty(state->cursor.x * 2 + 2, state->cursor.y * 2 + 4);
                state->cursor.y--;
                mark_tile_dirty(state->cursor.x * 2 + 1, state->cursor.y * 2 + 3);
                mark_tile_dirty(state->cursor.x * 2 + 2, state->cursor.y * 2 + 3);
                mark_tile_dirty(state->cursor.x * 2 + 1, state->cursor.y * 2 + 4);
                mark_tile_dirty(state->cursor.x * 2 + 2, state->cursor.y * 2 + 4);
            }
            break;
            
        case INPUT_DPAD_DOWN:
            if (state->cursor.y < 1) {
                mark_tile_dirty(state->cursor.x * 2 + 1, state->cursor.y * 2 + 3);
                mark_tile_dirty(state->cursor.x * 2 + 2, state->cursor.y * 2 + 3);
                mark_tile_dirty(state->cursor.x * 2 + 1, state->cursor.y * 2 + 4);
                mark_tile_dirty(state->cursor.x * 2 + 2, state->cursor.y * 2 + 4);
                state->cursor.y++;
                mark_tile_dirty(state->cursor.x * 2 + 1, state->cursor.y * 2 + 3);
                mark_tile_dirty(state->cursor.x * 2 + 2, state->cursor.y * 2 + 3);
                mark_tile_dirty(state->cursor.x * 2 + 1, state->cursor.y * 2 + 4);
                mark_tile_dirty(state->cursor.x * 2 + 2, state->cursor.y * 2 + 4);
            }
            break;
            
        case INPUT_DPAD_LEFT:
            if (state->cursor.x > 0) {
                mark_tile_dirty(state->cursor.x * 2 + 1, state->cursor.y * 2 + 3);
                mark_tile_dirty(state->cursor.x * 2 + 2, state->cursor.y * 2 + 3);
                mark_tile_dirty(state->cursor.x * 2 + 1, state->cursor.y * 2 + 4);
                mark_tile_dirty(state->cursor.x * 2 + 2, state->cursor.y * 2 + 4);
                state->cursor.x--;
                mark_tile_dirty(state->cursor.x * 2 + 1, state->cursor.y * 2 + 3);
                mark_tile_dirty(state->cursor.x * 2 + 2, state->cursor.y * 2 + 3);
                mark_tile_dirty(state->cursor.x * 2 + 1, state->cursor.y * 2 + 4);
                mark_tile_dirty(state->cursor.x * 2 + 2, state->cursor.y * 2 + 4);
            }
            break;
            
        case INPUT_DPAD_RIGHT:
            if (state->cursor.x < 2) {
                mark_tile_dirty(state->cursor.x * 2 + 1, state->cursor.y * 2 + 3);
                mark_tile_dirty(state->cursor.x * 2 + 2, state->cursor.y * 2 + 3);
                mark_tile_dirty(state->cursor.x * 2 + 1, state->cursor.y * 2 + 4);
                mark_tile_dirty(state->cursor.x * 2 + 2, state->cursor.y * 2 + 4);
                state->cursor.x++;
                mark_tile_dirty(state->cursor.x * 2 + 1, state->cursor.y * 2 + 3);
                mark_tile_dirty(state->cursor.x * 2 + 2, state->cursor.y * 2 + 3);
                mark_tile_dirty(state->cursor.x * 2 + 1, state->cursor.y * 2 + 4);
                mark_tile_dirty(state->cursor.x * 2 + 2, state->cursor.y * 2 + 4);
            }
            break;
            
        case INPUT_SELECT: {
            // Press the currently selected operator button
            const char* button_layout[2][3] = {
                {"C", "/", "*"},
                {"+", "-", "="}
            };
            
            if (state->cursor.y >= 0 && state->cursor.y <= 1 && 
                state->cursor.x >= 0 && state->cursor.x <= 2) {
                const char* label = button_layout[state->cursor.y][state->cursor.x];
                char op = label[0];
                handle_operator(state, op);
            }
            break;
        }
               
        default:
            break;
    }
}

static void calculator_destroy(Page *self) {
    if (self) {
        CalculatorState *state = (CalculatorState *)self->state;
        free(state);
        free(self);
    }
}

Page* calculator_page_create() {
    Page *page = malloc(sizeof(Page));
    CalculatorState *state = malloc(sizeof(CalculatorState));
    memset(state, 0, sizeof(CalculatorState));
    
    // Initialize calculator state
    clear_calculator(state);
    cursor_init(&state->cursor, 2, 1); // 3 cols (0-2), 2 rows (0-1)
    
    page->draw = calculator_draw;
    page->draw_tile = calculator_draw_tile;
    page->handle_input = calculator_handle_input;
    page->reset = NULL;
    page->destroy = calculator_destroy;
    page->state = state;
    
    return page;
}