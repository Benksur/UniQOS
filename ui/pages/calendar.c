#include "calendar.h"
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

typedef struct
{
    bool mounted;
    int display_month;
    int display_year;
} CalendarState;

static void calendar_draw(Page *self);
static void calendar_draw_tile(Page *self, int tx, int ty);
static void calendar_handle_input(Page *self, int event_type);
static void calendar_reset(Page *self);
static void calendar_destroy(Page *self);

// ========================= HELPERS ================================== //

// non-leap year)
static const int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static const char *month_names[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"};

static const char *day_names[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

static int is_leap_year(int year)
{
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

static int get_days_in_month(int month, int year)
{
    if (month == 2 && is_leap_year(year))
    {
        return 29;
    }
    return days_in_month[month - 1];
}

// zeller's congruence
static int get_day_of_week(int day, int month, int year)
{
    if (month < 3)
    {
        month += 12;
        year--;
    }
    int k = year % 100;
    int j = year / 100;
    int h = (day + ((13 * (month + 1)) / 5) + k + (k / 4) + (j / 4) - 2 * j) % 7;
    return (h + 6) % 7;
}

static void draw_calendar_title(CalendarState *state)
{
    char title[20];
    sprintf(title, "%s %d", month_names[state->display_month - 1], state->display_year);

    int title_width = strlen(title) * 6 * 2;
    int center_x = (TILE_WIDTH * TILE_COLS - title_width) / 2;
    display_draw_string(center_x, 50, title, current_theme.fg_colour, current_theme.bg_colour, 2);

    display_draw_string(10, 50, "<", current_theme.text_colour, current_theme.bg_colour, 2);
    display_draw_string(TILE_WIDTH * TILE_COLS - 20, 50, ">", current_theme.text_colour, current_theme.bg_colour, 2);
}

static void draw_day_headers()
{
    int start_x = 10;
    int start_y = 90;
    int day_width = (TILE_WIDTH * TILE_COLS - 20) / 7;

    for (int i = 0; i < 7; i++)
    {
        int x = start_x + i * day_width;
        display_draw_string(x + 5, start_y, day_names[i], current_theme.text_colour, current_theme.bg_colour, 1);
    }
}

static void draw_calendar_grid(CalendarState *state, RTC_DateTypeDef currentDate)
{
    int month = state->display_month;
    int year = state->display_year;
    int today = (month == currentDate.Month && year == 2000 + currentDate.Year) ? currentDate.Date : -1;

    int days_in_current_month = get_days_in_month(month, year);
    int first_day_of_week = get_day_of_week(1, month, year);

    int start_x = 10;
    int start_y = 110;
    int day_width = (TILE_WIDTH * TILE_COLS - 20) / 7;
    int day_height = 25;

    int current_day = 1;
    int row = 0;

    // Draw calendar grid
    for (row = 0; row < 6 && current_day <= days_in_current_month; row++)
    {
        for (int col = 0; col < 7; col++)
        {
            int x = start_x + col * day_width;
            int y = start_y + row * day_height;

            if (row == 0 && col < first_day_of_week)
            {
                continue;
            }

            if (current_day <= days_in_current_month)
            {
                char day_str[3];
                sprintf(day_str, "%d", current_day);

                if (current_day == today)
                {
                    display_fill_rect(x + 2, y, day_width - 4, day_height - 2, current_theme.accent_colour);
                    display_draw_string(x + 8, y + 8, day_str, current_theme.bg_colour, current_theme.accent_colour, 1);
                }
                else
                {
                    display_draw_string(x + 8, y + 8, day_str, current_theme.fg_colour, current_theme.bg_colour, 1);
                }

                current_day++;
            }
        }
    }

    // Draw grid lines
    for (int i = 0; i <= 7; i++)
    {
        int x = start_x + i * day_width;
        display_draw_vertical_line(x, start_y, start_y + row * day_height, current_theme.highlight_colour);
    }

    for (int i = 0; i <= row; i++)
    {
        int y = start_y + i * day_height;
        display_draw_horizontal_line(start_x, start_x + 7 * day_width, y, current_theme.highlight_colour);
    }
}

// ========================= VTABLE FUNCTIONS ========================= //

static void calendar_draw_tile(Page *self, int tx, int ty)
{
    CalendarState *state = (CalendarState *)self->state;

    if (!state->mounted)
    {
        display_fill_rect(0, 30, TILE_WIDTH * TILE_COLS, TILE_HEIGHT * (TILE_ROWS + 1), current_theme.bg_colour);

        RTC_TimeTypeDef sTime;
        RTC_DateTypeDef sDate;
        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

        if (state->display_month == 0)
        {
            state->display_month = sDate.Month;
            state->display_year = 2000 + sDate.Year;
        }

        draw_calendar_title(state);
        draw_day_headers();
        draw_calendar_grid(state, sDate);

        state->mounted = true;

        for (int y = 0; y < TILE_ROWS; y++)
        {
            for (int x = 0; x < TILE_COLS; x++)
            {
                mark_tile_clean(x, y);
            }
        }
    }
}

static void calendar_handle_input(Page *self, int event_type)
{
    CalendarState *state = (CalendarState *)self->state;
    bool need_redraw = false;

    switch (event_type)
    {
    case INPUT_DPAD_RIGHT:
        // Next month
        state->display_month++;
        if (state->display_month > 12)
        {
            state->display_month = 1;
            state->display_year++;
        }
        need_redraw = true;
        break;

    case INPUT_DPAD_LEFT:
        // Previous month
        state->display_month--;
        if (state->display_month < 1)
        {
            state->display_month = 12;
            state->display_year--;
        }
        need_redraw = true;
        break;

    default:
        break;
    }

    if (need_redraw)
    {
        state->mounted = false;
        mark_all_tiles_dirty();
    }
}

static void calendar_destroy(Page *self)
{
    if (self)
    {
        CalendarState *state = (CalendarState *)self->state;
        mem_free(state);
        mem_free(self);
    }
}

Page *calendar_page_create()
{
    Page *page = mem_malloc(sizeof(Page));
    CalendarState *state = mem_malloc(sizeof(CalendarState));
    memset(state, 0, sizeof(CalendarState));
    state->mounted = false;
    state->display_month = 0;
    state->display_year = 0;

    page->draw = NULL;
    page->draw_tile = calendar_draw_tile;
    page->handle_input = calendar_handle_input;
    page->reset = NULL;
    page->destroy = calendar_destroy;
    page->state = state;

    return page;
}