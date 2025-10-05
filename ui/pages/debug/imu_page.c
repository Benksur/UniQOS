#include "imu_page.h"
#include "screen.h"
#include "display.h"
#include "tile.h"
#include "input.h"
#include "theme.h"
#include "lsm6dsv.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#define TICK_TIME 2500 // ms

typedef struct
{
    uint32_t last_tick;
    lsm6dsv_data_t data;
    bool mounted;

} IMUState;

static void imu_draw_tile(Page *self, int tx, int ty)
{
    IMUState *state = (IMUState *)self->state;
    uint32_t curr_time = HAL_GetTick();
    int px, py;
    char buff[32];
    if (!state->mounted)
    {
        tile_to_pixels(0, 0, &px, &py);
        display_fill_rect(px, py, TILE_WIDTH * TILE_COLS, TILE_HEIGHT * TILE_ROWS, current_theme.bg_colour);
        state->mounted = true;
        state->last_tick = 0;
    }

    if ((curr_time - state->last_tick) > TICK_TIME)
    {
        lsm6dsv_get_all(&state->data);

        tile_to_pixels(0, 1, &px, &py);
        snprintf(buff, sizeof(buff), "ACC X: %8.3fg", state->data.ax);
        display_draw_string(px, py, buff, current_theme.text_colour, current_theme.bg_colour, 2);
        memset(buff, 0, sizeof(buff));

        tile_to_pixels(0, 2, &px, &py);
        snprintf(buff, sizeof(buff), "ACC Y: %8.3fg", state->data.ay);
        display_draw_string(px, py, buff, current_theme.text_colour, current_theme.bg_colour, 2);
        memset(buff, 0, sizeof(buff));

        tile_to_pixels(0, 3, &px, &py);
        snprintf(buff, sizeof(buff), "ACC Z: %8.3fg", state->data.az);
        display_draw_string(px, py, buff, current_theme.text_colour, current_theme.bg_colour, 2);
        memset(buff, 0, sizeof(buff));

        tile_to_pixels(0, 4, &px, &py);
        snprintf(buff, sizeof(buff), "GYR X: %6.3fdps", state->data.gx);
        display_draw_string(px, py, buff, current_theme.text_colour, current_theme.bg_colour, 2);
        memset(buff, 0, sizeof(buff));

        tile_to_pixels(0, 5, &px, &py);
        snprintf(buff, sizeof(buff), "GYR Y: %6.3fdps", state->data.gy);
        display_draw_string(px, py, buff, current_theme.text_colour, current_theme.bg_colour, 2);
        memset(buff, 0, sizeof(buff));

        tile_to_pixels(0, 6, &px, &py);
        snprintf(buff, sizeof(buff), "GYR Z: %6.3fdps", state->data.gz);
        display_draw_string(px, py, buff, current_theme.text_colour, current_theme.bg_colour, 2);
        memset(buff, 0, sizeof(buff));

        tile_to_pixels(0, 7, &px, &py);
        snprintf(buff, sizeof(buff), "TEMP: %9.3fc", state->data.temp);
        display_draw_string(px, py, buff, current_theme.text_colour, current_theme.bg_colour, 2);
        memset(buff, 0, sizeof(buff));

        state->last_tick = curr_time;
    }
    mark_all_tiles_dirty();
}

static void imu_handle_input(Page *self, int event_type)
{
    IMUState *state = (IMUState *)self->state;
    // switch (event_type)
    //     {
    //     case INPUT_DPAD_UP:
    //         state->d = UP;
    //         break;
    //     case INPUT_DPAD_DOWN:
    //         state->d = DOWN;
    //         break;
    //     case INPUT_DPAD_LEFT:
    //         state->d = LEFT;
    //         break;
    //     case INPUT_DPAD_RIGHT:
    //         state->d = RIGHT;
    //         break;
    //     default:
    //         break;
    //     }
    //     break;
    // default:
    //     break;
    // }
}

static void imu_destroy(Page *self)
{
    if (self)
    {
        IMUState *state = (IMUState *)self->state;
        free(state);
        free(self);
    }
}

Page *imu_page_create()
{
    Page *page = malloc(sizeof(Page));
    IMUState *state = malloc(sizeof(IMUState));
    memset(state, 0, sizeof(IMUState));
    state->mounted = false;

    page->draw = NULL;
    page->draw_tile = imu_draw_tile;
    page->handle_input = imu_handle_input;
    page->reset = NULL;
    page->destroy = imu_destroy;
    page->state = state;
    page->data_response = NULL;

    return page;
}
