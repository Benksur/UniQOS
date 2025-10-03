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
#include "memwrap.h"

#define CIRCLE_R ((TILE_WIDTH * 7) / 2)
#define CIRCLE_X ((TILE_WIDTH * TILE_COLS) / 2)
#define CIRCLE_Y 180
#define SECONDS_LENGTH (CIRCLE_R - 15)
#define MINUTES_LENGTH (CIRCLE_R - 30)
#define HOURS_LENGTH (CIRCLE_R - 50)
#define angle(x) ((x) * 6.0f - 90.0f) * 3.14159f / 180.0f

typedef enum
{
    ANALOG,
    DIGITAL
} ClockMode;

typedef struct
{
    uint32_t elapsed_time;
    ClockMode mode;
    uint8_t prev_minute;
    uint8_t prev_hour;
    uint8_t prev_second;
    bool mounted;
} ClockState;

static void clock_draw(Page *self);
static void clock_draw_tile(Page *self, int tx, int ty);
static void clock_handle_input(Page *self, int event_type);
static void clock_reset(Page *self);
static void clock_destroy(Page *self);

// ========================= HELPERS ================================== //

static void draw_clock_face()
{
    char *title = "Analog";
    int text_width = strlen(title) * 6 * 3; // size 3 font
    int center_x = (TILE_WIDTH * TILE_COLS - text_width) / 2;
    display_draw_string(center_x, 40, title, current_theme.fg_colour, current_theme.bg_colour, 3);
    display_draw_string(TILE_WIDTH * 7, 40, ">", current_theme.text_colour, current_theme.bg_colour, 3);
    display_draw_circle(CIRCLE_X, CIRCLE_Y, CIRCLE_R, current_theme.fg_colour);
    for (int i = 0; i < 60; i++)
    {
        float angle = angle(i);
        int inner_r = (i % 5 == 0) ? CIRCLE_R - 15 : CIRCLE_R - 10;
        int outer_r = CIRCLE_R;
        int start_x = CIRCLE_X + (int)(inner_r * cos(angle));
        int start_y = CIRCLE_Y + (int)(inner_r * sin(angle));
        int end_x = CIRCLE_X + (int)(outer_r * cos(angle));
        int end_y = CIRCLE_Y + (int)(outer_r * sin(angle));
        display_draw_line(start_x, start_y, end_x, end_y, current_theme.fg_colour);
    }
}

static void clear_minute_hand(uint8_t minute)
{
    float angle = angle(minute);
    int end_x = CIRCLE_X + (int)(MINUTES_LENGTH * cos(angle));
    int end_y = CIRCLE_Y + (int)(MINUTES_LENGTH * sin(angle));
    display_draw_line(CIRCLE_X, CIRCLE_Y, end_x, end_y, current_theme.bg_colour);
}

static void draw_minute_hand(uint8_t minute)
{
    float angle = angle(minute);
    int end_x = CIRCLE_X + (int)(MINUTES_LENGTH * cos(angle));
    int end_y = CIRCLE_Y + (int)(MINUTES_LENGTH * sin(angle));
    display_draw_line(CIRCLE_X, CIRCLE_Y, end_x, end_y, current_theme.fg_colour);
}

static void clear_hour_hand(uint8_t hour)
{
    float angle = angle((hour % 12) * 5);
    int end_x = CIRCLE_X + (int)(HOURS_LENGTH * cos(angle));
    int end_y = CIRCLE_Y + (int)(HOURS_LENGTH * sin(angle));
    display_draw_line(CIRCLE_X, CIRCLE_Y, end_x, end_y, current_theme.bg_colour);
}

static void draw_hour_hand(uint8_t hour)
{
    float angle = angle((hour % 12) * 5);
    int end_x = CIRCLE_X + (int)(HOURS_LENGTH * cos(angle));
    int end_y = CIRCLE_Y + (int)(HOURS_LENGTH * sin(angle));
    display_draw_line(CIRCLE_X, CIRCLE_Y, end_x, end_y, current_theme.fg_colour);
}

static void draw_curr_second_hand(uint8_t second)
{
    float angle = angle(second);
    float angle_prev = angle((second - 1 + 60) % 60);
    int end_x = CIRCLE_X + (int)(SECONDS_LENGTH * cos(angle_prev));
    int end_y = CIRCLE_Y + (int)(SECONDS_LENGTH * sin(angle_prev));
    display_draw_line(CIRCLE_X, CIRCLE_Y, end_x, end_y, current_theme.bg_colour);
    end_x = CIRCLE_X + (int)(SECONDS_LENGTH * cos(angle));
    end_y = CIRCLE_Y + (int)(SECONDS_LENGTH * sin(angle));
    display_draw_line(CIRCLE_X, CIRCLE_Y, end_x, end_y, current_theme.accent_colour);
}

// Clear previously drawn second hand (even if seconds were skipped) and draw the current one
static void update_second_hand(ClockState *state, uint8_t second)
{
    if (state->prev_second != 255 && state->prev_second != second)
    {
        float angle_prev = angle(state->prev_second);
        int px = CIRCLE_X + (int)(SECONDS_LENGTH * cos(angle_prev));
        int py = CIRCLE_Y + (int)(SECONDS_LENGTH * sin(angle_prev));
        display_draw_line(CIRCLE_X, CIRCLE_Y, px, py, current_theme.bg_colour);
    }

    float ang = angle(second);
    int x = CIRCLE_X + (int)(SECONDS_LENGTH * cos(ang));
    int y = CIRCLE_Y + (int)(SECONDS_LENGTH * sin(ang));
    display_draw_line(CIRCLE_X, CIRCLE_Y, x, y, current_theme.accent_colour);

    state->prev_second = second;
}

static void draw_digital_minutes(RTC_TimeTypeDef sTime)
{
    char m_buffer[3] = {0};
    sprintf(m_buffer, "%02d", sTime.Minutes);
    int m_width = strlen(m_buffer) * 6 * 10;
    int m_center_x = (TILE_WIDTH * TILE_COLS - m_width) / 2;
    display_draw_string(m_center_x, 180, m_buffer, current_theme.fg_colour, current_theme.bg_colour, 10);
}

static void draw_digital_seconds(RTC_TimeTypeDef sTime)
{
    char sec_buffer[3] = {0};
    sprintf(sec_buffer, "%02d", sTime.Seconds);
    int sec_width = strlen(sec_buffer) * 6 * 5;
    int sec_x = (TILE_WIDTH * TILE_COLS - sec_width) / 2;
    display_draw_string(sec_x, 270, sec_buffer, current_theme.accent_colour, current_theme.bg_colour, 5);
}

static void draw_digital_hours(RTC_TimeTypeDef sTime)
{
    char h_buffer[3] = {0};
    sprintf(h_buffer, "%02d", sTime.Hours);
    int h_width = strlen(h_buffer) * 6 * 10;
    int h_center_x = (TILE_WIDTH * TILE_COLS - h_width) / 2;
    display_draw_string(h_center_x, 80, h_buffer, current_theme.fg_colour, current_theme.bg_colour, 10);
}

// ========================= VTABLE FUNCTIONS ========================= //

static void clock_draw_tile(Page *self, int tx, int ty)
{
    ClockState *state = (ClockState *)self->state;
    uint32_t current_time = HAL_GetTick();

    char time_buffer[15] = {0};
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN); // need to read date to refresh shadow registers

    if ((current_time - state->elapsed_time) >= 1000)
    {
        // Time has changed, update display

        if (state->mode == DIGITAL)
        {
            if (!state->mounted)
            {
                display_fill_rect(0, 30, TILE_WIDTH * TILE_COLS, TILE_HEIGHT * (TILE_ROWS + 1), current_theme.bg_colour);
                char *title = "Digital";
                int text_width = strlen(title) * 6 * 3;
                int center_x = (TILE_WIDTH * TILE_COLS - text_width) / 2;
                display_draw_string(center_x, 40, title, current_theme.fg_colour, current_theme.bg_colour, 3);
                display_draw_string(10, 40, "<", current_theme.text_colour, current_theme.bg_colour, 3);
                draw_digital_hours(sTime);
                draw_digital_minutes(sTime);
                state->mounted = true;
            }
            draw_digital_seconds(sTime);
            if (sTime.Hours != state->prev_hour)
                draw_digital_hours(sTime);
            if (sTime.Minutes != state->prev_minute)
                draw_digital_minutes(sTime);
        }
        else
        {
            if (!state->mounted)
            {
                display_fill_rect(0, 30, TILE_WIDTH * TILE_COLS, TILE_HEIGHT * (TILE_ROWS + 1), current_theme.bg_colour);
                draw_clock_face();
                state->mounted = true;
                // Initialize prev_second to current to avoid clearing a wrong second on first update
                state->prev_second = sTime.Seconds;
            }
            update_second_hand(state, sTime.Seconds);

            if (sTime.Hours != state->prev_hour)
            {
                clear_hour_hand(sTime.Hours == 0 ? 11 : sTime.Hours - 1);
            }
            draw_hour_hand(sTime.Hours);
            if (sTime.Minutes != state->prev_minute)
            {
                clear_minute_hand(sTime.Minutes == 0 ? 59 : sTime.Minutes - 1);
            }
            draw_minute_hand(sTime.Minutes);
        }

        state->elapsed_time = current_time;
        state->prev_minute = sTime.Minutes;
        state->prev_hour = sTime.Hours;
    }
    mark_all_tiles_dirty();
}

static void clock_handle_input(Page *self, int event_type)
{
    ClockState *state = (ClockState *)self->state;
    switch (event_type)
    {
    case INPUT_DPAD_RIGHT:
        if (state->mode != DIGITAL)
        {
            state->mode = DIGITAL;
            state->elapsed_time = 0;
            state->mounted = false;
        }
        break;
    case INPUT_DPAD_LEFT:
        if (state->mode != ANALOG)
        {
            state->mode = ANALOG;
            state->elapsed_time = 0;
            state->mounted = false;
        }
        break;
    default:
        break;
    }
}

static void clock_destroy(Page *self)
{
    if (self)
    {
        ClockState *state = (ClockState *)self->state;
        mem_free(state);
        mem_free(self);
    }
}

Page *clock_page_create()
{
    Page *page = mem_malloc(sizeof(Page));
    ClockState *state = mem_malloc(sizeof(ClockState));
    memset(state, 0, sizeof(ClockState));
    state->elapsed_time = 0;
    state->mode = ANALOG;
    state->prev_minute = 255;
    state->prev_hour = 255;
    state->prev_second = 255;
    state->mounted = false;

    page->draw = NULL;
    page->draw_tile = clock_draw_tile;
    page->handle_input = clock_handle_input;
    page->reset = NULL;
    page->destroy = clock_destroy;
    page->state = state;

    return page;
}