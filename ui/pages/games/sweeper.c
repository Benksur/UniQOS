#include "snake.h"
#include "screen.h"
#include "display.h"
#include "tile.h"
#include "input.h"
#include "cursor.h"
#include "theme.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#define GRID_SIZE_X TILE_COLS
#define GRID_SIZE_Y TILE_ROWS

#define MAX_MINES 20

#define TICK_TIME 100 // ms - can be super low redrawing is handled manually

// Values 0-8 used for mine proximity counts
#define CELL_MINE 0x0A

#define CURSOR_COLOUR current_theme.fg_colour
#define CELL_BORDER_COLOR current_theme.accent_colour

uint8_t mine_symbol[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,
	0x00,0x10,0x41,0x00,0x9F,0x20,0x07,0xFC,0x00,0x7F,0xC0,0x0F,
	0xFE,0x00,0xFF,0xE0,0x3F,0xFF,0x80,0xFF,0xE0,0x0F,0xFE,0x00,
	0x7F,0xC0,0x07,0xFC,0x00,0x9F,0x20,0x10,0x41,0x00,0x04,0x00,
	0x00,0x00,0x00,0x00,0x00};


uint8_t flag_symbol[] = {0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x3C,
	0x00,0x03,0xF8,0x00,0x3F,0xE0,0x03,0xFF,0x00,0x3F,0xC0,0x03,
	0xF0,0x00,0x38,0x00,0x02,0x00,0x00,0x20,0x00,0x02,0x00,0x00,
	0x20,0x00,0x02,0x00,0x00,0x20,0x00,0x02,0x00,0x00,0x20,0x00,
	0x00,0x00,0x00,0x00,0x00};

typedef enum
{
    GAME_START,
    GAME_ALIVE,
    GAME_OVER,
    GAME_WIN,
} GameState;

typedef struct
{
    uint8_t cell_val;
    bool is_hidden;
    bool has_flag;
    bool redraw;
} Cell;

typedef struct
{
    uint32_t last_tick;
    Cursor cursor;
    int old_cursor_x;
    int old_cursor_y;
    Cell grid[GRID_SIZE_Y][GRID_SIZE_X];
    uint8_t mine_count;
    GameState game_state;
} SweeperState;

/*
 * Initialise the board
 */
void board_reveal(SweeperState *state, uint8_t reveal_x, uint8_t reveal_y, bool sub_call)
{
    uint8_t nearbyFlags = 0;

    // Reveal on flag is nothing
    if (state->grid[reveal_y][reveal_x].has_flag == true)
    {
        return;
    }

    // Reveal mine game over
    if (state->grid[reveal_y][reveal_x].cell_val == CELL_MINE)
    {
        state->grid[reveal_y][reveal_x].is_hidden = false;
        state->game_state = GAME_OVER;
        state->grid[reveal_y][reveal_x].redraw = true;
        return;
    }

    // Reveal on empty revealed is nothing
    if (state->grid[reveal_y][reveal_x].is_hidden == false &&
        state->grid[reveal_y][reveal_x].cell_val == 0)
    {
        return;
    }

    // reveal on revealed square with val
    if (state->grid[reveal_y][reveal_x].is_hidden == false && !sub_call)
    {
        // check flag count
        for (uint8_t i = -1; i < 2; i++)
        {
            for (uint8_t j = -1; j < 2; j++)
            {
                // Skip OOB
                if (reveal_y + j >= GRID_SIZE_Y || reveal_x + i >= GRID_SIZE_X ||
                    reveal_y + j < 0 || reveal_x + i < 0)
                {
                    continue;
                }
                if (state->grid[reveal_y + j][reveal_x + i].has_flag)
                {
                    nearbyFlags++;
                }
            }
        }

        // call reveal on surrounds
        // I think this logic may be dogy might need to check
        if (nearbyFlags == state->grid[reveal_y][reveal_x].cell_val)
        {
            for (uint8_t i = -1; i < 2; i++)
            {
                for (uint8_t j = -1; j < 2; j++)
                {
                    // Skip OOB
                    if (reveal_y + j >= GRID_SIZE_Y ||
                        reveal_x + i >= GRID_SIZE_X || reveal_y + j < 0 ||
                        reveal_x + i < 0)
                    {
                        continue;
                    }
                    board_reveal(state, i + reveal_x, j + reveal_y, true);
                }
            }
        }

        return;
    }

    // revealing 0 reveals all surrounding
    if (state->grid[reveal_y][reveal_x].cell_val == 0)
    {
        state->grid[reveal_y][reveal_x].is_hidden = false;
        state->grid[reveal_y][reveal_x].redraw = true;
        for (uint8_t i = -1; i < 2; i++)
        {
            for (uint8_t j = -1; j < 2; j++)
            {
                // Skip OOB
                if (reveal_y + j >= GRID_SIZE_Y || reveal_x + i >= GRID_SIZE_X ||
                    reveal_y + j < 0 || reveal_x + i < 0)
                {
                    continue;
                }
                board_reveal(state, i + reveal_x, j + reveal_y, true);
            }
        }
    }
    else
    { // Otherwise reveal plain num
        state->grid[reveal_y][reveal_x].is_hidden = false;
        state->grid[reveal_y][reveal_x].redraw = true;
    }

    // Check for win state
    bool has_won = true;
    for (uint8_t row = 0; row < GRID_SIZE_Y; row++)
    {
        for (uint8_t col = 0; col < GRID_SIZE_X; col++)
        {
            // mines should be flagged
            if (state->grid[row][col].cell_val == CELL_MINE && !state->grid[row][col].has_flag)
            {
                has_won = false;
                break;
            }
            // all other cells should be revealed
            if (state->grid[row][col].cell_val != CELL_MINE && state->grid[row][col].is_hidden)
            {
                has_won = false;
                break;
            }
        }
        if (!has_won)
            break;
    }

    if (has_won)
    {
        state->game_state = GAME_WIN;
    }
}

/*
 * Initialise the board
 */
void board_init(SweeperState *state, uint8_t reveal_x, uint8_t reveal_y)
{
    uint8_t mine_count = 0;

    // Add mines
    while (mine_count < MAX_MINES)
    {
        uint8_t mine_x = rand() % GRID_SIZE_X;
        uint8_t mine_y = rand() % GRID_SIZE_Y;

        // skip mines in 3x3 near first
        if (mine_x >= reveal_x - 1 && mine_x <= reveal_x + 1 &&
            mine_y >= reveal_y - 1 && mine_y <= reveal_y + 1)
        {
            continue;
        }

        if (state->grid[mine_y][mine_x].cell_val != CELL_MINE)
        {
            state->grid[mine_y][mine_x].cell_val = CELL_MINE;
            mine_count++;
        }
    }

    // Set non-mine values and hide
    for (uint8_t row = 0; row < GRID_SIZE_Y; row++)
    {
        for (uint8_t col = 0; col < GRID_SIZE_X; col++)
        {
            if (state->grid[row][col].cell_val == CELL_MINE)
            {
                continue;
            }

            // Calc Mines nearby
            for (uint8_t i = -1; i < 2; i++)
            {
                for (uint8_t j = -1; j < 2; j++)
                {
                    // Skip OOB
                    if (row + j >= GRID_SIZE_Y || col + i >= GRID_SIZE_X ||
                        row + j < 0 || col + i < 0)
                    {
                        continue;
                    }
                    if (state->grid[row + j][col + i].cell_val == CELL_MINE)
                    {
                        state->grid[row][col].cell_val++;
                    }
                }
            }
        }
    }
}

/*
 * Initialise snake in game
 */
void init_game(SweeperState *state)
{

    state->last_tick = HAL_GetTick();

    state->game_state = GAME_START;

    // manual cursor init
    state->cursor.max_x = GRID_SIZE_X - 1;
    state->cursor.max_y = GRID_SIZE_Y - 1;
    state->cursor.x = GRID_SIZE_X / 2;
    state->cursor.y = GRID_SIZE_Y / 2;
    state->cursor.selected = false;

    // hide board & reset
    for (uint8_t row = 0; row < GRID_SIZE_Y; row++)
    {
        for (uint8_t col = 0; col < GRID_SIZE_X; col++)
        {
            state->grid[row][col].is_hidden = true;
            state->grid[row][col].has_flag = false;
            state->grid[row][col].cell_val = 0;
            state->grid[row][col].redraw = true;
        }
    }

    srand(state->last_tick);
}

void sweeper_handle_input(Page *self, int event_type)
{
    SweeperState *state = self->state;
    if (state->game_state == GAME_START && event_type == INPUT_SELECT)
    {
        // first reveal
        board_init(state, state->cursor.x, state->cursor.y);
        state->game_state = GAME_ALIVE;
        board_reveal(state, state->cursor.x, state->cursor.y, false);
    }
    else if (state->game_state == GAME_OVER && event_type == INPUT_SELECT)
    {
        // new game
        init_game(state);
    }
    else if (state->game_state == GAME_WIN && event_type == INPUT_SELECT)
    {
        // new game
        init_game(state);
    }
    else if (event_type == INPUT_SELECT)
    {
        board_reveal(state, state->cursor.x, state->cursor.y, false);
    }
    else if (event_type == INPUT_KEYPAD_2 && state->grid[state->cursor.y][state->cursor.x].is_hidden == true)
    {
        // TODO confirm keypad 2 is suitable
        state->grid[state->cursor.y][state->cursor.x].has_flag = !state->grid[state->cursor.y][state->cursor.x].has_flag;
    }
    else if (event_type == INPUT_DPAD_LEFT)
    {
        cursor_move(&state->cursor, -1, 0, &state->old_cursor_x, &state->old_cursor_y);
    }
    else if (event_type == INPUT_DPAD_RIGHT)
    {
        cursor_move(&state->cursor, 1, 0, &state->old_cursor_x, &state->old_cursor_y);
    }
    else if (event_type == INPUT_DPAD_UP)
    {
        cursor_move(&state->cursor, 0, 1, &state->old_cursor_x, &state->old_cursor_y);
    }
    else if (event_type == INPUT_DPAD_DOWN)
    {
        cursor_move(&state->cursor, 0, -1, &state->old_cursor_x, &state->old_cursor_y);
    }
}

void draw_cell(Cell *cell, uint16_t x, uint16_t y)
{
    // Default cell border
    display_draw_rect(x * TILE_WIDTH, y * TILE_HEIGHT, TILE_WIDTH, 
        TILE_HEIGHT, CELL_BORDER_COLOR);

    // Draw flag
    if (cell->has_flag)
    {
        display_draw_bits(x + 5, y + 5, flag_symbol, current_theme.text_colour, 
            current_theme.bg_colour, 20, 20);
    }
    else if (!cell->is_hidden)
    {
        // default reveal background
        display_fill_rect(x * TILE_WIDTH + 1, y * TILE_HEIGHT + 1,
                          TILE_WIDTH - 2, TILE_HEIGHT - 2, current_theme.highlight_colour);
        if (cell->cell_val == CELL_MINE)
        {
            display_draw_bits(x + 5, y + 5, mine_symbol, current_theme.text_colour, 
                current_theme.highlight_colour,20, 20);
        }
        else if (cell->cell_val != 0)
        {
            // Val type reveal
            display_draw_char(x * TILE_WIDTH + 10, y * TILE_HEIGHT + 8,
                              '0' + cell->cell_val, current_theme.text_colour,
                              current_theme.highlight_colour, 2);
        }
    }
    else
    {
        display_fill_rect(x * TILE_WIDTH + 1, y * TILE_HEIGHT + 1,
                          TILE_WIDTH - 2, TILE_HEIGHT - 2, current_theme.bg_colour);
    }
}

void draw_cursor(uint16_t x, uint16_t y)
{
    display_draw_rect(x * TILE_WIDTH, y * TILE_HEIGHT, TILE_WIDTH,
                      TILE_HEIGHT, CURSOR_COLOUR);
    display_draw_rect(x * TILE_WIDTH + 1, y * TILE_HEIGHT + 1,
                      TILE_WIDTH - 2, TILE_HEIGHT - 2, CURSOR_COLOUR);
}

static void sweeper_draw_tile(Page *self, int tx, int ty)
{
    SweeperState *state = self->state;
    uint32_t curr_time = HAL_GetTick();

    if ((curr_time - state->last_tick) > TICK_TIME)
    {
        // Redraw all required grids
        for (uint8_t row = 0; row < GRID_SIZE_Y; row++)
        {
            for (uint8_t col = 0; col < GRID_SIZE_X; col++)
            {
                if (state->grid[row][col].redraw)
                {
                    draw_cell(&state->grid[row][col], col, row);
                }
            }
        }

        // redraw cursor and reset old location
        if (state->old_cursor_x != state->cursor.x && 
            state->old_cursor_y != state->cursor.y)
        {
            draw_cell(&state->grid[state->old_cursor_y][state->old_cursor_x], 
                state->old_cursor_x, state->old_cursor_y);
            draw_cursor(state->cursor.x, state->cursor.y);
            state->old_cursor_x = state->cursor.x;
            state->old_cursor_y = state->cursor.y;
        }

        state->last_tick = curr_time;
    }

    // check game state
    if (state->game_state == GAME_WIN)
    {
        char *title = "GAME WIN";
        int text_width = strlen(title) * 6 * 3;
        uint16_t center_x = (TILE_WIDTH * TILE_COLS - text_width) / 2;
        uint16_t center_y = (TILE_HEIGHT * TILE_ROWS) / 2;
        display_draw_string(center_x, center_y, title, 
            current_theme.text_colour, current_theme.bg_colour, 3);
    }
    else if (state->game_state == GAME_OVER)
    {
        char *title = "GAME OVER";
        int text_width = strlen(title) * 6 * 3;
        uint16_t center_x = (TILE_WIDTH * TILE_COLS - text_width) / 2;
        uint16_t center_y = (TILE_HEIGHT * TILE_ROWS) / 2;
        display_draw_string(center_x, center_y, title, 
            current_theme.text_colour, current_theme.bg_colour, 3);
    }

    mark_all_tiles_dirty();
}

static void sweeper_destroy(Page *self)
{
    if (self)
    {
        SweeperState *state = (SweeperState *)self->state;
        free(state);
        free(self);
    }
}

Page *sweeper_page_create()
{
    Page *page = malloc(sizeof(Page));
    SweeperState *state = malloc(sizeof(SweeperState));
    memset(state, 0, sizeof(SweeperState));

    init_game(state);

    page->draw = NULL;
    page->draw_tile = sweeper_draw_tile;
    page->handle_input = sweeper_handle_input;
    page->reset = NULL;
    page->destroy = sweeper_destroy;
    page->state = state;

    return page;
}