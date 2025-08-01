#include "screen.h"
#include "navbar.h"
#include "tile.h"
#include "input.h"

#include "menu.h"

static Page* current_page;

void screen_init(Page* initial_page) {
    current_page = initial_page;
    current_page->draw();
}

void screen_set_page(Page* new_page) {
    current_page = new_page;
    mark_all_tiles_dirty();
    current_page->draw();
}

void screen_handle_input(int event_type) {
    if (current_page && current_page->handle_input) {
        current_page->handle_input(event_type);
    }
}

void screen_tick(void) {
    flush_dirty_tiles(current_page->draw_tile);
}
