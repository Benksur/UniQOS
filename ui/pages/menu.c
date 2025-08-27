#include "menu.h"
#include "screen.h"
#include "display.h"
#include "tile.h"
#include "menu_row.h"
#include "cursor.h"
#include "input.h"
#include <stddef.h>

#define MENU_ITEMS_COUNT 7

typedef struct {
    Cursor cursor;
    const char* items[MENU_ITEMS_COUNT];
} MenuState;

// forward declarations
static void menu_draw(Page* self);
static void menu_draw_tile(int tx, int ty, Page* self);
static void menu_handle_input(int event_type, Page* self);
static void menu_reset(Page* self);

// state is static since only one menu page exists
static MenuState menu_state = {
    .cursor = {0, 0, 0, MENU_ITEMS_COUNT - 1, false},
    .items = {"Phone", "SMS", "Contacts", "Clock", "Calculator", "Calendar", "Settings"}
};
// --- Draw functions ---
static void menu_draw(Page* self) {
    MenuState* s = (MenuState*)self->state;
    for (int ty = 0; ty < MENU_ITEMS_COUNT; ty++) {
        draw_menu_row(ty, ty == s->cursor.y, s->items[ty]);
    }
}

static void menu_draw_tile(int tx, int ty, Page* self) {
    MenuState* s = (MenuState*)self->state;
    bool highlight = (s->cursor.y == ty);
    draw_menu_row(ty, highlight, s->items[ty]);
}

// --- Input ---
static void menu_handle_input(int event_type, Page* self) {
    MenuState* s = (MenuState*)self->state;

    int old_x, old_y;
    bool moved = false;

    switch(event_type) {
        case INPUT_DPAD_UP:    moved = cursor_move(&s->cursor, 0, -1, &old_x, &old_y); break;
        case INPUT_DPAD_DOWN:  moved = cursor_move(&s->cursor, 0,  1, &old_x, &old_y); break;
        case INPUT_SELECT: s->cursor.selected = true; break;
    }

    // mark old/new tiles dirty
    if (moved) {
        mark_tile_dirty(old_x, old_y);
        mark_tile_dirty(s->cursor.x, s->cursor.y);
    }

    // Navigation action on select
    if (event_type == INPUT_SELECT) {
        switch (s->cursor.y) {
            case 0: /* screen_set_page(&phone_page); */ break;
            case 1: /* screen_set_page(&sms_page); */ break;
            case 2: /* screen_set_page(&contacts_page); */ break;
            case 3: /* screen_set_page(&clock_page); */ break;
            case 4: /* screen_set_page(&calc_page); */ break;
            case 5: /* screen_set_page(&calendar_page); */ break;
            case 6: /* screen_set_page(&settings_page); */ break;
        }
        s->cursor.selected = false; // reset selection
    }
}

static void menu_reset(Page* self) {
    MenuState* s = (MenuState*)self->state;
    cursor_reset(&s->cursor);
}

Page menu_page = {
    .draw = menu_draw,
    .draw_tile = menu_draw_tile,
    .handle_input = menu_handle_input,
    .reset = menu_reset,
    .destroy = NULL,      // static singleton page
    .state = &menu_state
};