#include "menu.h"
#include "screen.h"
#include "display.h"
#include "tile.h"
#include "menu_row.h"
#include "cursor.h"
#include "input.h"
#include <stddef.h>

#define MENU_ITEMS_COUNT 7
#define MENU_VISIBLE_COUNT 5

typedef struct
{
    Cursor cursor;
    const char *items[MENU_ITEMS_COUNT];
    int page_offset;
} MenuState;

// forward declarations
static void menu_draw();
static void menu_draw_tile(int tx, int ty);
static void menu_handle_input(int event_type);
static void menu_reset();

// state is static since only one menu page exists
static MenuState menu_state = {
    .cursor = {0, 0, 0, MENU_ITEMS_COUNT - 1, false},
    .items = {"Phone", "SMS", "Contacts", "Clock", "Calculator", "Calendar", "Settings"},
    .page_offset = 0};
// --- Draw functions ---
static void menu_draw()
{
    for (int i = 0; i < MENU_VISIBLE_COUNT; i++)
    {
        int item_index = menu_state.page_offset + i;
        if (item_index >= MENU_ITEMS_COUNT)
            break;

        int tile_y = i * 2; // screen row (0–4) mapped to tiles
        if (item_index < MENU_ITEMS_COUNT)
        {
            bool highlight = (menu_state.cursor.y == item_index);
            draw_menu_row(tile_y, highlight, menu_state.items[item_index]);
        }
        else
        {
            draw_empty_row(tile_y);
        }
    }
}

static void menu_draw_tile(int tx, int ty)
{
    int visible_row = ty / 2; // 0–4 on screen
    int item_index = menu_state.page_offset + visible_row;

    if (item_index < MENU_ITEMS_COUNT)
    {
        bool highlight = (menu_state.cursor.y == item_index);
        draw_menu_row(visible_row * 2, highlight, menu_state.items[item_index]);
    }
    else
    {
        draw_empty_row(visible_row * 2);
    }

    for (int i = 0; i < TILE_COLS; i++)
    {
        mark_tile_clean(i, visible_row * 2);
        mark_tile_clean(i, visible_row * 2 + 1);
    }
}

static void mark_row_dirty(int row) {
    int tile_y = row * 2;
    for (int i = 0; i < TILE_COLS; i++) {
        mark_tile_dirty(i, tile_y);
        mark_tile_dirty(i, tile_y + 1);
    }
}

static void update_page_offset() {
    // scroll down
    if (menu_state.cursor.y >= menu_state.page_offset + MENU_VISIBLE_COUNT) {
        menu_state.page_offset = menu_state.cursor.y; // new page start
        mark_all_tiles_dirty();
    }
    // scroll up
    else if (menu_state.cursor.y < menu_state.page_offset) {
        menu_state.page_offset = 0; // back to first page
        mark_all_tiles_dirty();
    }
}

static void menu_handle_input(int event_type) {
    int old_x, old_y;
    bool moved = false;

    // --- Movement ---
    switch (event_type) {
        case INPUT_DPAD_UP:
            moved = cursor_move(&menu_state.cursor, 0, -1, &old_x, &old_y);
            break;
        case INPUT_DPAD_DOWN:
            moved = cursor_move(&menu_state.cursor, 0, +1, &old_x, &old_y);
            break;
        case INPUT_DPAD_RIGHT: // jump to page 2 (items 5+)
            if (menu_state.cursor.y < 5)
                moved = cursor_move(&menu_state.cursor, 0, 5 - menu_state.cursor.y, &old_x, &old_y);
            break;
        case INPUT_DPAD_LEFT: // jump back to top
            if (menu_state.cursor.y >= 5)
                moved = cursor_move(&menu_state.cursor, 0, -menu_state.cursor.y, &old_x, &old_y);
            break;
        case INPUT_SELECT:
            menu_state.cursor.selected = true;
            break;
    }

    // --- Page / redraw updates ---
    if (moved) {
        update_page_offset();

        int old_row = old_y - menu_state.page_offset;
        int new_row = menu_state.cursor.y - menu_state.page_offset;

        if (old_row >= 0 && old_row < MENU_VISIBLE_COUNT) mark_row_dirty(old_row);
        if (new_row >= 0 && new_row < MENU_VISIBLE_COUNT) mark_row_dirty(new_row);
    }

    // --- Selection action ---
    if (event_type == INPUT_SELECT) {
        menu_state.cursor.selected = false;
        switch (menu_state.cursor.y) {
            case 0: /* screen_set_page(&phone_page);     */ break;
            case 1: /* screen_set_page(&sms_page);       */ break;
            case 2: /* screen_set_page(&contacts_page);  */ break;
            case 3: /* screen_set_page(&clock_page);     */ break;
            case 4: /* screen_set_page(&calc_page);      */ break;
            case 5: /* screen_set_page(&calendar_page);  */ break;
            case 6: /* screen_set_page(&settings_page);  */ break;
        }
    }
}


static void menu_reset()
{
    cursor_reset(&menu_state.cursor);
}

Page menu_page = {
    .draw = menu_draw,
    .draw_tile = menu_draw_tile,
    .handle_input = menu_handle_input,
    .reset = menu_reset,
    .destroy = NULL, // static singleton page
    .state = &menu_state};