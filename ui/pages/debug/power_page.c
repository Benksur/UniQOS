#include "power_page.h"
#include "screen.h"
#include "display.h"
#include "tile.h"
#include "input.h"
#include "theme.h"
#include "mcp73871.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#define TICK_TIME 5000 // ms

typedef struct
{
    uint32_t last_tick;
    uint16_t soc;
    int16_t current;
    uint16_t voltage;
    uint16_t capacity_avail;
    uint16_t capacity_full;
    uint16_t health;
    bool mounted;

} PowerState;



static void power_draw_tile(Page *self, int tx, int ty)
{
    PowerState *state = (PowerState *)self->state;
    uint32_t curr_time = HAL_GetTick();
    int px, py;
    char buff[32];
    if (!state->mounted) {
        tile_to_pixels(0, 0, &px, &py);
        display_fill_rect(px, py, TILE_WIDTH * TILE_COLS, TILE_HEIGHT * TILE_ROWS, current_theme.bg_colour);
        state->mounted = true;
        state->last_tick = 0;
    }

    if ((curr_time - state->last_tick) > TICK_TIME)
    {
        screen_request(PAGE_REQUEST_BATTERY_HC, NULL);

        tile_to_pixels(0, 1, &px, &py);

        snprintf(buff, sizeof(buff), "SOC: %13hd%%", state->soc);
        display_draw_string(px, py, buff, current_theme.text_colour, current_theme.bg_colour, 2);
        memset(buff, 0, sizeof(buff));

        tile_to_pixels(0, 2, &px, &py);
        snprintf(buff, sizeof(buff), "Voltage: %8hdmV", state->voltage);
        display_draw_string(px, py, buff, current_theme.text_colour, current_theme.bg_colour, 2);
        memset(buff, 0, sizeof(buff));

        tile_to_pixels(0, 3, &px, &py);
        snprintf(buff, sizeof(buff), "Current: %8hdmA", state->current);
        display_draw_string(px, py, buff, current_theme.text_colour, current_theme.bg_colour, 2);
        memset(buff, 0, sizeof(buff));

        tile_to_pixels(0, 4, &px, &py);
        snprintf(buff, sizeof(buff), "R Capacity: %4hdmAh", state->capacity_avail);
        display_draw_string(px, py, buff, current_theme.text_colour, current_theme.bg_colour, 2);
        memset(buff, 0, sizeof(buff));

        tile_to_pixels(0, 5, &px, &py);
        snprintf(buff, sizeof(buff), "T Capacity: %4hdmAh", state->capacity_full);
        display_draw_string(px, py, buff, current_theme.text_colour, current_theme.bg_colour, 2);
        memset(buff, 0, sizeof(buff));

        tile_to_pixels(0, 6, &px, &py);
        snprintf(buff, sizeof(buff), "Health: %10hd%%", (uint8_t)state->health);
        display_draw_string(px, py, buff, current_theme.text_colour, current_theme.bg_colour, 2);
        memset(buff, 0, sizeof(buff));

        tile_to_pixels(0, 7, &px, &py);

        enum MCP73871_States charge_status = mcp73871_status();
        switch (charge_status)
        {
        case CHARGING:
            display_draw_string(px, py, "Status:    CHARGING", current_theme.text_colour, current_theme.bg_colour, 2);


            if(state->current > 0){
                tile_to_pixels(0, 8, &px, &py);
                //value in min
                uint16_t time_remain = ((state->capacity_full-state->capacity_avail)*60)/(state->current);
                snprintf(buff, sizeof(buff), "Remain: %8hdmin", time_remain);
                display_draw_string(px, py, buff, current_theme.text_colour, current_theme.bg_colour, 2);  
            }

            break;
        case STANDBY:
            display_draw_string(px, py, "Status:     STANDBY", current_theme.text_colour, current_theme.bg_colour, 2);

            // need to ensure in sync
            if(state->current < 0){
                tile_to_pixels(0, 8, &px, &py);
                //value in min
                uint16_t time_remain = (state->capacity_avail*60)/(-state->current);
                snprintf(buff, sizeof(buff), "Remain: %8hdmin", time_remain);
                display_draw_string(px, py, buff, current_theme.text_colour, current_theme.bg_colour, 2); 
            }

            break;
        case LOW_BATTERY_OUT:
            display_draw_string(px, py, "Status: LOW_BATTERY", current_theme.text_colour, current_theme.bg_colour, 2);
            break;
        case SHUTDOWN:
            display_draw_string(px, py, "Status:    SHUTDOWN", current_theme.text_colour, current_theme.bg_colour, 2);
            break;
        case FAULT:
            display_draw_string(px, py, "Status:      FAULT", current_theme.text_colour, current_theme.bg_colour, 2);
            break;
        case CHARGE_COMPLETE:
            display_draw_string(px, py, "Status: CHARGE DONE", current_theme.text_colour, current_theme.bg_colour, 2);
            break;
        default:
            display_draw_string(px, py, "Status:     UNKNOWN", current_theme.text_colour, current_theme.bg_colour, 2);
            break;
        }

        state->last_tick = curr_time;
    }
    mark_all_tiles_dirty();
}

static void power_handle_input(Page *self, int event_type)
{
    PowerState *state = (PowerState *)self->state;
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

static void power_handle_response(Page *self, int type, void *resp) {
    // VERY UNSAFE MEMORY HANDLING DO NOT COPY PLEASE FIX
    PowerState* state = (PowerState *)self->state;
    uint16_t* stats = (uint16_t*)resp;
    if (type == PAGE_RESPONSE_BATTERY_HC) {
        state->soc = stats[0];
        state->current = stats[1];
        state->voltage = stats[2];
        state->capacity_avail = stats[3];
        state->capacity_full = stats[4];
        state->health = stats[5];
    }
}

static void power_destroy(Page *self)
{
    if (self)
    {
        PowerState *state = (PowerState *)self->state;
        free(state);
        free(self);
    }
}

Page *power_page_create()
{
    Page *page = malloc(sizeof(Page));
    PowerState *state = malloc(sizeof(PowerState));
    memset(state, 0, sizeof(PowerState));
    state->mounted = false;

    page->draw = NULL;
    page->draw_tile = power_draw_tile;
    page->handle_input = power_handle_input;
    page->reset = NULL;
    page->destroy = power_destroy;
    page->state = state;
    page->data_response = power_handle_response;

    return page;
}
