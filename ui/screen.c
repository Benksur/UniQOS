#include "screen.h"
#include "tile.h"
#include <stdlib.h>

#define MAX_PAGE_STACK 10

static Page* page_stack[MAX_PAGE_STACK];
static int page_top = -1;
static Page* current_page = NULL;

/**
 * Initialize the screen with the initial page.
 */
void screen_init(Page* initial_page) {
    page_top = -1;
    current_page = initial_page;
    if (current_page) {
        if (current_page->reset) current_page->reset(current_page);
        current_page->draw(current_page);
    }
}

/**
 * Push a new page onto the stack.
 */
void screen_push_page(Page* new_page) {
    if (page_top < MAX_PAGE_STACK - 1) {
        page_stack[++page_top] = current_page;
        current_page = new_page;
        if (current_page->reset) current_page->reset(current_page);
        mark_all_tiles_dirty();
        if (current_page->draw) current_page->draw(current_page);
    }
}

/**
 * Pop the current page off the stack.
 */
void screen_pop_page(void) {
    if (page_top >= 0) {
        // Free current page if dynamic
        if (current_page && current_page->destroy) {
            current_page->destroy(current_page);
        }

        current_page = page_stack[page_top--];
        if (current_page->reset) current_page->reset(current_page);
        mark_all_tiles_dirty();
        if (current_page->draw) current_page->draw(current_page);
    }
}

/**
 * Replace the current page with a new page.
 */
void screen_set_page(Page* new_page) {
    // Free current page if dynamic
    if (current_page && current_page->destroy) {
        current_page->destroy(current_page);
    }

    current_page = new_page;
    if (current_page && current_page->reset) {
        current_page->reset(current_page);
    }
    mark_all_tiles_dirty();
    if (current_page && current_page->draw) {
        current_page->draw(current_page);
    }
}

/**
 * Handle input for the current page.
 */
void screen_handle_input(int event_type) {
    if (!current_page) return;

    // Let the page handle input (page may access its state)
    if (current_page->handle_input) {
        current_page->handle_input(event_type, current_page);
    }
}

/**
 * Flush all dirty tiles.
 */
void screen_tick(void) {
    if (!current_page) return;
    if (current_page->draw_tile) {
        flush_dirty_tiles(current_page->draw_tile);
    }
}