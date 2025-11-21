#include "games.h"
#include "screen.h"
#include "display.h"
#include "tile.h"
#include "menu_row.h"
#include "cursor.h"
#include "input.h"
#include "theme.h"
#include "snake.h"
#include "sweeper.h"
#include "memwrap.h"
#include <stddef.h>
#include <string.h>

#define GAMES_ITEMS_COUNT 2
#define GAMES_VISIBLE_COUNT 5

typedef struct
{
    Cursor cursor;
    const char *items[GAMES_ITEMS_COUNT];
    int page_offset;
} GamesState;

// Forward declarations
static void games_draw(Page *self);
static void games_draw_tile(Page *self, int tx, int ty);
static void games_handle_input(Page *self, int event_type);
static void games_reset(Page *self);
static void games_destroy(Page *self);

// --- Helper functions ---
static void draw_games_header(int tile_y)
{
    int px, py;
    tile_to_pixels(0, tile_y, &px, &py);
    int width = TILE_WIDTH * TILE_COLS;
    int height = TILE_HEIGHT * 2;

    display_fill_rect(px, py, width, height, current_theme.bg_colour);

    const char *header_text = "Games";
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
static void games_draw(Page *self) {}

static void games_draw_tile(Page *self, int tx, int ty)
{
    GamesState *state = (GamesState *)self->state;
    int visible_row = ty / 2; // 0-4 on screen

    if (visible_row == 0)
    {
        // Header row
        draw_games_header(0);
    }
    else if (visible_row <= GAMES_ITEMS_COUNT)
    {
        // Game option rows
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

static void games_handle_input(Page *self, int event_type)
{
    GamesState *state = (GamesState *)self->state;
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
            Page *snake_page = snake_page_create();
            screen_push_page(snake_page);
            break;
        }
        case 1:
        {
            // Page *sweeper_page = sweeper_page_create();
            // screen_push_page(sweeper_page);
            break;
        }
        }
    }
}

static void games_reset(Page *self)
{
    GamesState *state = (GamesState *)self->state;
    // cursor_reset(&state->cursor);
}

static void games_destroy(Page *self)
{
    if (self)
    {
        GamesState *state = (GamesState *)self->state;
        mem_free(state);
        mem_free(self);
    }
}

// --- Public API ---
Page *games_page_create()
{
    Page *page = mem_malloc(sizeof(Page));
    GamesState *state = mem_malloc(sizeof(GamesState));

    // Initialize cursor (0 to GAMES_ITEMS_COUNT-1)
    state->cursor = (Cursor){0, 0, 0, GAMES_ITEMS_COUNT - 1, false};

    // Initialize game options
    state->items[0] = "Snake";
    state->items[1] = "Sweeper";
    state->page_offset = 0;

    page->draw = games_draw;
    page->draw_tile = games_draw_tile;
    page->handle_input = games_handle_input;
    page->reset = games_reset;
    page->destroy = games_destroy;
    page->state = state;

    return page;
}
