#include "status_bar.h"

typedef struct
{
    bool mounted;
    RTC_TimeTypeDef last_time;
    uint8_t last_signal_strength;
    uint8_t last_battery_level;

    bool volume_indicator_visible;
    uint8_t current_volume;
    uint32_t volume_show_start_time;
    uint32_t volume_show_duration_ms;
} StatusBarState;

static StatusBarState status_state = {0};

void draw_status_bar(void)
{
    if (!status_state.mounted)
    {
        display_fill_rect(0, 0, 240, 25, current_theme.fg_colour);
        status_state.mounted = true;
        status_state.volume_show_duration_ms = 2000;
    }
}

static void update_time_display(void)
{
    if (!status_state.mounted)
        return;

    // read both registers to avoid locking out the date register
    RTC_DateTypeDef sDate;
    RTC_TimeTypeDef sTime;
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

    if (sTime.Hours != status_state.last_time.Hours ||
        sTime.Minutes != status_state.last_time.Minutes)
    {

        char time_buffer[16];
        sprintf(time_buffer, "%02d:%02d", sTime.Hours, sTime.Minutes);
        display_draw_string(10, 5, time_buffer, current_theme.bg_colour, current_theme.fg_colour, 2);

        status_state.last_time = sTime;
    }
}

static void update_volume_indicator(void)
{
    if (!status_state.mounted || !status_state.volume_indicator_visible)
        return;

    uint32_t current_time = HAL_GetTick();
    if (current_time - status_state.volume_show_start_time >= status_state.volume_show_duration_ms)
    {
        display_fill_rect(90, 5, 60, 16, current_theme.fg_colour);
        status_state.volume_indicator_visible = false;
        return;
    }

    // Only redraw if volume changed (this function is called every tick)
    static uint8_t last_drawn_volume = 0;
    if (status_state.current_volume != last_drawn_volume)
    {
        char volume_buffer[16];
        sprintf(volume_buffer, "%d%%", status_state.current_volume);
        int text_width = strlen(volume_buffer) * 6;
        int center_x = (240 - text_width) / 2;
        int center_y = (25 - 8) / 2;
        display_fill_rect(90, 5, 60, 16, current_theme.fg_colour);
        display_draw_string(center_x, center_y, volume_buffer, current_theme.bg_colour, current_theme.fg_colour, 1);
        last_drawn_volume = status_state.current_volume;
    }
}

void status_bar_show_volume(uint8_t volume)
{
    if (!status_state.mounted)
        return;

    status_state.current_volume = volume;
    status_state.volume_indicator_visible = true;
    status_state.volume_show_start_time = HAL_GetTick();

    char volume_buffer[16];
    sprintf(volume_buffer, "%d%%", volume);
    int text_width = strlen(volume_buffer) * 6;
    int center_x = (240 - text_width) / 2;
    int center_y = (25 - 8) / 2;
    display_draw_string(center_x, center_y, volume_buffer, current_theme.bg_colour, current_theme.fg_colour, 1);
}

void status_bar_tick(void)
{
    update_time_display();
    update_volume_indicator();
}

void status_bar_update_signal(uint8_t strength)
{

    if (strength != status_state.last_signal_strength)
    {
        display_fill_rect(180, 5, 30, 16, current_theme.fg_colour);

        display_draw_signal_bars(180, 5, strength, current_theme.bg_colour, current_theme.fg_colour);

        status_state.last_signal_strength = strength;
    }
}

void status_bar_update_battery(uint8_t level)
{

    if (level != status_state.last_battery_level)
    {
        display_fill_rect(210, 6, 30, 16, current_theme.fg_colour);

        display_draw_battery_icon(210, 6, level, current_theme.bg_colour, current_theme.fg_colour);

        status_state.last_battery_level = level;
    }
}

void status_bar_reset(void)
{
    memset(&status_state, 0, sizeof(status_state));
}