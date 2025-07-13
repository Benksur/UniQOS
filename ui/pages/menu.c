#include "menu.h"
#include "screen.h"
#include "input.h"
#include "tile.h"
#include "menu_row.h"

static int selected_index = 0;

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
    // Draw all rows, but only even rows will be processed by draw_menu_row
    for (int i = 0; i < TILE_ROWS; i++) {
        draw_menu_row(i, i == selected_index);
    }
}

static void draw_tile(int tile_x, int tile_y) {

}

static void handle_input(int event_type, int x, int y) {
    // Cycle through all even rows (0, 2, 4, 6)
    selected_index += 2;
    if (selected_index >= TILE_ROWS || selected_index % 2 != 0) {
        selected_index = 0;
    }
    // int old_selected_index = selected_index;
    
    // switch (event_type) {
    //     case INPUT_UP:
    //         if (selected_index >= 4) {
    //             selected_index -= 4;
    //         }
    //         break;
            
    //     case INPUT_DOWN:
    //         if (selected_index + 4 < NUM_ITEMS) {
    //             selected_index += 4;
    //         }
    //         break;
            
    //     case INPUT_LEFT:
    //         if (selected_index % 4 > 0) {
    //             selected_index--;
    //         }
    //         break;
            
    //     case INPUT_RIGHT:
    //         if (selected_index % 4 < 3 && selected_index + 1 < NUM_ITEMS) {
    //             selected_index++;
    //         }
    //         break;
            
    //     case INPUT_SELECT:
    //         navigate_to_page(selected_index);
    //         break;
            
    //     case INPUT_BACK:
    //         // do nothing, since menu is root
    //         break;
    // }
    // if (old_selected_index != selected_index) {
    //     mark_tile_dirty(old_selected_index % 4, old_selected_index / 4);
    //     mark_tile_dirty(selected_index % 4, selected_index / 4);
    // }
}

Page menu_page = {
    .draw = draw,
    .draw_tile = draw_tile,
    .handle_input = handle_input
};
