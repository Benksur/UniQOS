#include "clock.h"
#include "screen.h"
#include "display.h"
#include "tile.h"
#include "input.h"
#include "theme.h"

#include "rtc.h"
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

typedef struct {
    uint32_t elapsed_time;
    bool mounted;
} ClockState;

static void clock_draw(Page *self);
static void clock_draw_tile(Page *self, int tx, int ty);
static void clock_handle_input(Page *self, int event_type);
static void clock_reset(Page *self);
static void clock_destroy(Page *self);

static void clock_draw_tile(Page *self, int tx, int ty)
{
    ClockState *state = (ClockState *)self->state;
    uint32_t current_time = HAL_GetTick();

    char time_buffer[15] = {0};
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN); // need to read date to refresh shadow registers
    
    if ((current_time - state->elapsed_time) >= 1000) {
        // Time has changed, update display
        if (!state->mounted) {
            display_fill_rect(0, 30, TILE_WIDTH * TILE_COLS, TILE_HEIGHT * (TILE_ROWS + 1), current_theme.bg_colour);
            state->mounted = true;
        }
        sprintf(time_buffer, "%02d:%02d:%02d", sTime.Hours, sTime.Minutes, sTime.Seconds);
        int text_width = strlen(time_buffer) * 6 * 3; // size 3 font
        int center_x = (TILE_WIDTH * TILE_COLS - text_width) / 2;
        display_draw_string(center_x, 50, time_buffer, current_theme.fg_colour, current_theme.bg_colour, 3);

        int cr = (TILE_WIDTH * 7) / 2;
        int cx = (TILE_WIDTH * TILE_COLS) / 2;
        int cy = 180;
        display_draw_circle(cx, cy, cr, current_theme.fg_colour);

        float angle = (sTime.Seconds * 6.0f - 90.0f) * 3.14159f / 180.0f;
        float angle_prev = ((sTime.Seconds - 1) * 6.0f - 90.0f) * 3.14159f / 180.0f;
        int length = cr - 10; // slightly shorter than radius
        int end_x = cx + (int)(length * cos(angle_prev));
        int end_y = cy + (int)(length * sin(angle_prev));
        display_draw_line(cx, cy, end_x, end_y, current_theme.bg_colour);
        end_x = cx + (int)(length * cos(angle));
        end_y = cy + (int)(length * sin(angle));
        display_draw_line(cx, cy, end_x, end_y, current_theme.fg_colour);

        float minute_angle = (sTime.Minutes * 6.0f - 90.0f) * 3.14159f / 180.0f;
        float minute_angle_prev = ((sTime.Minutes - 1) * 6.0f - 90.0f) * 3.14159f / 180.0f;
        int minute_length = cr - 40;
        int minute_end_x = cx + (int)(minute_length * cos(minute_angle_prev));
        int minute_end_y = cy + (int)(minute_length * sin(minute_angle_prev));
        display_draw_line(cx, cy, minute_end_x, minute_end_y, current_theme.bg_colour);
        minute_end_x = cx + (int)(minute_length * cos(minute_angle));
        minute_end_y = cy + (int)(minute_length * sin(minute_angle));
        display_draw_line(cx, cy, minute_end_x, minute_end_y, current_theme.fg_colour);
        
        state->elapsed_time = current_time;
    }
    mark_all_tiles_dirty();
    
}

static void clock_destroy(Page *self)
{
    if (self)
    {
        ClockState* state = (ClockState*)self->state;
        free(state);
        free(self->state);
    }
}

Page *clock_page_create()
{
    Page *page = malloc(sizeof(Page));
    ClockState *state = malloc(sizeof(ClockState));
    memset(state, 0, sizeof(ClockState));
    state->elapsed_time = 0;
    state->mounted = false;

    page->draw = NULL;
    page->draw_tile = clock_draw_tile;
    page->handle_input = NULL;
    page->reset = NULL;
    page->destroy = clock_destroy;
    page->state = state;

    return page;
}