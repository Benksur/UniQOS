#include "snake.h"
#include "screen.h"
#include "display.h"
#include "tile.h"
#include "input.h"
#include "theme.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#define GRID_SIZE_X TILE_COLS
#define GRID_SIZE_Y TILE_ROWS
#define MAX_SNAKE GRID_SIZE_X *GRID_SIZE_Y

#define TICK_TIME 500 // ms

#define APPLE_COLOUR current_theme.accent_colour
#define SNAKE_COLOUR current_theme.fg_colour

typedef enum
{
    GAME_WIN,
    GAME_OVER,
    GAME_RUN,
    GAME_NEW,
} GameState;

typedef enum
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
} Direction;

typedef struct
{
    uint32_t last_tick;
    GameState game_state;
    uint8_t snake_x[MAX_SNAKE]; // using last index as prev tail
    uint8_t snake_y[MAX_SNAKE];
    uint8_t snake_len;
    uint8_t apple_x;
    uint8_t apple_y;
    bool new_apple;
    Direction d;
} SnakeState;

/*
 * Runs a single game tick to do the following:
 *      - Move the snake in direction `currMove`
 *      - End game on self collision
 *      - Extend snake on collision with apple
 */
void game_tick(SnakeState *state)
{

    // copy out old tail
    state->snake_x[MAX_SNAKE - 1] = state->snake_x[state->snake_len - 1];
    state->snake_y[MAX_SNAKE - 1] = state->snake_y[state->snake_len - 1];

    // advance snake body
    for (int i = state->snake_len - 1; i > 0; i--)
    {
        state->snake_x[i] = state->snake_x[i - 1];
        state->snake_y[i] = state->snake_y[i - 1];
    }

    // advance head
    switch (state->d)
    {
    case LEFT:
        state->snake_x[0]--;
        break;

    case RIGHT:
        state->snake_x[0]++;
        break;

    case UP:
        state->snake_y[0]--;
        break;

    case DOWN:
        state->snake_y[0]++;
        break;

    default:
        break;
    }

    // check OOB
    if (state->snake_x[0] < 0)
    {
        state->snake_x[0] = GRID_SIZE_X - 1;
    }
    else if (state->snake_x[0] >= GRID_SIZE_X)
    {
        state->snake_x[0] = 0;
    }
    else if (state->snake_y[0] < 0)
    {
        state->snake_y[0] = GRID_SIZE_Y - 1;
    }
    else if (state->snake_y[0] >= GRID_SIZE_Y)
    {
        state->snake_y[0] = 0;
    }

    // check collision on snake
    for (int i = 1; i < state->snake_len; i++)
    {
        if (state->snake_x[0] == state->snake_x[i] && state->snake_y[0] == state->snake_y[i])
        {
            state->game_state = GAME_OVER;
            return;
        }
    }

    // collision edge case for size 2
    if (state->snake_len == 2 && state->snake_x[0] == state->snake_x[MAX_SNAKE - 1] && state->snake_y[0] == state->snake_y[MAX_SNAKE - 1])
    {
        state->game_state = GAME_OVER;
        return;
    }

    // check collision on apple and extend
    if (state->snake_x[0] == state->apple_x && state->snake_y[0] == state->apple_y)
    {
        // extend
        state->snake_x[state->snake_len] = state->snake_x[MAX_SNAKE - 1];
        state->snake_y[state->snake_len] = state->snake_y[MAX_SNAKE - 1];
        state->snake_len++;

        // Check win
        if (state->snake_len == MAX_SNAKE)
        {
            state->game_state == GAME_WIN;
        }
        else
        {
            state->new_apple = true;
        }
    }

    // new apple
    if (state->new_apple)
    {
        // new apple
        bool safe_spawn = true;
        do
        {
            state->apple_x = (uint8_t)(rand() % GRID_SIZE_X);
            state->apple_y = (uint8_t)(rand() % GRID_SIZE_Y);

            for (uint8_t i = 0; i < state->snake_len; i++)
            {
                if (state->apple_x == state->snake_x[i] && state->apple_y == state->snake_y[i])
                {
                    safe_spawn = false;
                    break;
                }
            }

        } while (!safe_spawn);
    }

    return;
}

/*
 * Init the game with a new grid
 */
void init_game(SnakeState *state)
{
    state->last_tick = HAL_GetTick();
    state->game_state = GAME_RUN;
    state->snake_len = 1;
    state->snake_x[0] = GRID_SIZE_X / 2;
    state->snake_y[0] = GRID_SIZE_Y / 2;
    state->new_apple = true;
    srand(state->last_tick);
} 

static void snake_draw_tile(Page *self, int tx, int ty)
{
    SnakeState *state = (SnakeState *)self->state;
    uint32_t curr_time = HAL_GetTick();
    uint16_t x_draw, y_draw;

    // Init a new game and reset display
    if (state->game_state == GAME_NEW) {
        init_game(state);

        // Clear display
        display_fill_rect(0, y_draw, TILE_WIDTH * TILE_COLS, TILE_HEIGHT * TILE_ROWS, current_theme.bg_colour);
    }

    // TODO: should probably confirm that this logic holds at tick reset
    if ((curr_time - state->last_tick) > TICK_TIME)
    {
        game_tick(state);

        // Clear tail
        x_draw = (uint16_t)state->snake_x[MAX_SNAKE - 1] * TILE_WIDTH;
        y_draw = (uint16_t)state->snake_y[MAX_SNAKE - 1] * TILE_HEIGHT;
        display_fill_rect(x_draw, y_draw, TILE_WIDTH, TILE_HEIGHT, current_theme.bg_colour);

        // Draw head
        x_draw = (uint16_t)state->snake_x[0] * TILE_WIDTH;
        y_draw = (uint16_t)state->snake_y[0] * TILE_HEIGHT;
        display_fill_rect(x_draw, y_draw, TILE_WIDTH, TILE_HEIGHT, SNAKE_COLOUR);

        // Draw Apple if req
        if (state->new_apple)
        {
            x_draw = (uint16_t)state->apple_x * TILE_WIDTH;
            y_draw = (uint16_t)state->apple_y * TILE_HEIGHT;
            display_fill_rect(x_draw, y_draw, TILE_WIDTH, TILE_HEIGHT, APPLE_COLOUR);
            state->new_apple = false;
        }

        state->last_tick = curr_time;
    }

    // Handle game states
    // TODO: May want to check these I'm sorta just guessing here
    if (state->game_state == GAME_WIN)
    {
        char *title = "GAME WIN";
        int text_width = strlen(title) * 6 * 3;
        uint16_t center_x = (TILE_WIDTH * TILE_COLS - text_width) / 2;
        uint16_t center_y = (TILE_HEIGHT * TILE_ROWS) / 2;
        display_draw_string(center_x, center_y, title, current_theme.text_colour, current_theme.bg_colour, 3);
    }
    else if (state->game_state == GAME_OVER)
    {
        char *title = "GAME OVER";
        int text_width = strlen(title) * 6 * 3;
        uint16_t center_x = (TILE_WIDTH * TILE_COLS - text_width) / 2;
        uint16_t center_y = (TILE_HEIGHT * TILE_ROWS) / 2;
        display_draw_string(center_x, center_y, title, current_theme.text_colour, current_theme.bg_colour, 3);
    }

    mark_all_tiles_dirty();
}

static void snake_handle_input(Page *self, int event_type)
{
    SnakeState *state = (SnakeState *)self->state;
    switch (state->game_state)
    {
    case GAME_OVER:
        if (event_type == INPUT_DPAD_UP || event_type == INPUT_DPAD_DOWN ||
            event_type == INPUT_DPAD_LEFT || event_type == INPUT_DPAD_RIGHT ||
            event_type == INPUT_SELECT)
        {
            state->game_state = GAME_NEW;
        }
        break;
    case GAME_WIN:
        if (event_type == INPUT_DPAD_UP || event_type == INPUT_DPAD_DOWN ||
            event_type == INPUT_DPAD_LEFT || event_type == INPUT_DPAD_RIGHT ||
            event_type == INPUT_SELECT)
        {
            state->game_state = GAME_NEW;
        }
        break;
    case GAME_RUN:
        switch (event_type)
        {
        case INPUT_DPAD_UP:
            state->d = UP;
            break;
        case INPUT_DPAD_DOWN:
            state->d = DOWN;
            break;
        case INPUT_DPAD_LEFT:
            state->d = LEFT;
            break;
        case INPUT_DPAD_RIGHT:
            state->d = RIGHT;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

static void snake_destroy(Page *self)
{
    if (self)
    {
        SnakeState *state = (SnakeState *)self->state;
        free(state);
        free(self);
    }
}

Page *snake_page_create()
{
    Page *page = malloc(sizeof(Page));
    SnakeState *state = malloc(sizeof(SnakeState));
    memset(state, 0, sizeof(SnakeState));

    init_game(state);

    page->draw = NULL;
    page->draw_tile = snake_draw_tile;
    page->handle_input = snake_handle_input;
    page->reset = NULL;
    page->destroy = snake_destroy;
    page->state = state;

    return page;
}
