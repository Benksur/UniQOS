#include "menu.h"
#include "screen.h"
#include "display.h"
#include "input.h"
#include "tile.h"
#include "menu_row.h"

static int selected_index = 0;

static const char* menu_items[] = {
    "Phone",
    "SMS",
    "Contacts",
    "Clock",
    "Calculator",
    "Calendar",
    "Settings"
};

static const char* icon_files[] = {
    "phone.dat",
    "sms.dat",
    "contacts.dat",
    "clock.dat",
    "calc.dat",
    "calendar.dat",
    "settings.dat"
};

static inline int tile_to_index(int tx, int ty) {
    return ty * 4 + tx;
}

static void navigate_to_page(int index) {
    switch (index) {
        case 0: 
            // screen_set_page(&phone_page);
            break;
        case 1:
            // screen_set_page(&sms_page); 
            break;
        case 2:
            // screen_set_page(&contacts_page);
            break;
        case 3:
            // screen_set_page(&clock_page);
            break;
        case 4:
            // screen_set_page(&calc_page);
            break;
        case 5:
            // screen_set_page(&calendar_page);
            break;
        case 6: 
            // screen_set_page(&settings_page);
            break;
        default:
            break;
    }
}


static void draw(void) {
    for (int i = 0; i < TILE_ROWS * TILE_ROWS; i++) {
        int tx = i % TILE_COLS;
        int ty = i / TILE_COLS;
        if (tx) continue;
        draw_menu_row(ty, ty == selected_index, menu_items[ty/2]);
    }
}

static void draw_tile(int tile_x, int tile_y) {
    if (tile_x == 0) {
        int base_row = (tile_y % 2 == 0) ? tile_y : tile_y - 1;
        draw_menu_row(base_row, base_row == selected_index, menu_items[base_row/2]);
    }
}

static void handle_input(int event_type, int x, int y) {
    int old_selected_index = selected_index;
    selected_index += 2;
    if (selected_index >= TILE_ROWS || selected_index % 2 != 0) {
        selected_index = 0;
    }

    if (old_selected_index != selected_index) {
        mark_tile_dirty(0, old_selected_index);
        mark_tile_dirty(0, old_selected_index + 1);

        mark_tile_dirty(0, selected_index);
        mark_tile_dirty(0, selected_index + 1);
    }
}

Page menu_page = {
    .draw = draw,
    .draw_tile = draw_tile,
    .handle_input = handle_input
};
