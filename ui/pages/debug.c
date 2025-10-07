#include "debug.h"
#include "screen.h"
#include "display.h"
#include "tile.h"
#include "menu_row.h"
#include "cursor.h"
#include "input.h"
#include "theme.h"
#include "power_page.h"
#include "imu_page.h"
#include "memwrap.h"
#include <stddef.h>
#include <string.h>

#define DEBUG_ITEMS_COUNT 2
#define DEBUG_VISIBLE_COUNT 5

typedef struct
{
    Cursor cursor;
    const char *items[DEBUG_ITEMS_COUNT];
    int page_offset;
} DebugState;

// Forward declarations
static void debug_draw(Page *self);
static void debug_draw_tile(Page *self, int tx, int ty);
static void debug_handle_input(Page *self, int event_type);
static void debug_reset(Page *self);
static void debug_destroy(Page *self);

// --- Helper functions ---
static void draw_debug_header(int tile_y)
{
    int px, py;
    tile_to_pixels(0, tile_y, &px, &py);
    int width = TILE_WIDTH * TILE_COLS;
    int height = TILE_HEIGHT * 2;

    display_fill_rect(px, py, width, height, current_theme.bg_colour);

    const char *header_text = "Debug";
    int text_width = strlen(header_text) * 6 * 3; // size 3 font
    int center_x = px + (width - text_width) / 2;
    display_draw_string(center_x, py + 15, header_text, current_theme.fg_colour, current_theme.bg_colour, 3);

    // Draw bottom border
    display_draw_horizontal_line(px, py + height - 1, px + width, current_theme.highlight_colour);
}

static void mark_row_dirty(int row)
{
    int tile_y = row * 2;
    for (int i = 0; i < TILE_COLS; i++)
    {
        mark_tile_dirty(i, tile_y);
        mark_tile_dirty(i, tile_y + 1);
    }
}

// --- Draw functions ---
static void debug_draw(Page *self) {}

static void debug_draw_tile(Page *self, int tx, int ty)
{
    DebugState *state = (DebugState *)self->state;
    int visible_row = ty / 2; // 0-4 on screen

    if (visible_row == 0)
    {
        // Header row
        draw_debug_header(0);
    }
    else if (visible_row <= DEBUG_ITEMS_COUNT)
    {
        // Debug option rows
        int item_index = visible_row - 1;
        bool highlight = (state->cursor.y == item_index);
        draw_menu_row(visible_row * 2, highlight, state->items[item_index]);
    }
    else
    {
        // Empty rows
        draw_empty_row(visible_row * 2);
    }

    // Mark tiles as clean
    for (int i = 0; i < TILE_COLS; i++)
    {
        mark_tile_clean(i, visible_row * 2);
        mark_tile_clean(i, visible_row * 2 + 1);
    }
}

static void debug_handle_input(Page *self, int event_type)
{
    DebugState *state = (DebugState *)self->state;
    int old_x, old_y;
    bool moved = false;

    // --- Movement ---
    switch (event_type)
    {
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

    // --- Update display if cursor moved ---
    if (moved)
    {
        // Mark old and new positions dirty (rows start from 1, not 0 because of header)
        mark_row_dirty(old_y + 1);
        mark_row_dirty(state->cursor.y + 1);
    }

    // --- Selection action ---
    if (event_type == INPUT_SELECT)
    {
        state->cursor.selected = false;
        switch (state->cursor.y)
        {
        case 0:
        {
            Page *power_page = power_page_create();
            screen_push_page(power_page);
            break;
        }
        case 1:
        {
            Page *imu_page = imu_page_create();
            screen_push_page(imu_page);
            break;
        }
        }
    }
}

static void debug_reset(Page *self)
{
    DebugState *state = (DebugState *)self->state;
    // cursor_reset(&state->cursor);
}

static void debug_destroy(Page *self)
{
    if (self)
    {
        DebugState *state = (DebugState *)self->state;
        mem_free(state);
        mem_free(self);
    }
}

// --- Public API ---
Page *debug_page_create()
{
    Page *page = mem_malloc(sizeof(Page));
    DebugState *state = mem_malloc(sizeof(DebugState));

    // Initialize cursor (0 to DEBUG_ITEMS_COUNT-1)
    state->cursor = (Cursor){0, 0, 0, DEBUG_ITEMS_COUNT - 1, false};

    // Initialize debug options
    state->items[0] = "Power";
    state->items[1] = "IMU";
    state->page_offset = 0;

    page->draw = debug_draw;
    page->draw_tile = debug_draw_tile;
    page->handle_input = debug_handle_input;
    page->reset = debug_reset;
    page->destroy = debug_destroy;
    page->state = state;

    return page;
}
