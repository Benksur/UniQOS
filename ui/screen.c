#include "screen.h"
#include "tile.h"
#include <stdlib.h>

#define MAX_PAGE_STACK 10

static Page* page_stack[MAX_PAGE_STACK];
static int page_top = -1;
static Page* current_page = NULL;
static DataRequestFn data_request_fn = NULL;
static DataResponseFn data_response_fn = NULL;

void screen_set_data_req_fn(DataRequestFn fn) {
    data_request_fn = fn;
}

void screen_set_data_resp_fn(DataResponseFn fn) {
    data_response_fn = fn;
}

/**
 * Initialize the screen with the initial page.
 */
void screen_init(Page* initial_page) {
    page_top = -1;
    current_page = initial_page;

    if (current_page) {
        if(data_request_fn) {
            current_page->data_request = data_request_fn;
        }
        if (current_page->reset) current_page->reset();
        current_page->draw();
    }
}

/**
 * Push a new page onto the stack.
 */
void screen_push_page(Page* new_page) {
    if (page_top < MAX_PAGE_STACK - 1) {

        page_stack[++page_top] = current_page;

        current_page = new_page;

        if (current_page && data_request_fn) {
            current_page->data_request = data_request_fn;
        }

        if (current_page->reset) current_page->reset();
        mark_all_tiles_dirty();
        if (current_page->draw) current_page->draw();
    }
}

/**
 * Pop the current page off the stack.
 */
void screen_pop_page(void) {
    if (page_top >= 0) {
        // Free current page if dynamic
        if (current_page && current_page->destroy) {
            current_page->destroy();
        }

        if (current_page && data_request_fn) {
            current_page->data_request = data_request_fn;
        }

        current_page = page_stack[page_top--];
        if (current_page->reset) current_page->reset();
        mark_all_tiles_dirty();
        if (current_page->draw) current_page->draw();
    }
}

/**
 * Replace the current page with a new page.
 */
void screen_set_page(Page* new_page) {
    // Free current page if dynamic
    if (current_page && current_page->destroy) {
        current_page->destroy();
    }

    current_page = new_page;

    if (current_page && data_request_fn) {
        current_page->data_request = data_request_fn;
    }

    if (current_page && current_page->reset) {
        current_page->reset();
    }
    mark_all_tiles_dirty();
    if (current_page && current_page->draw) {
        current_page->draw();
    }
}

/**
 * Handle input for the current page.
 */
void screen_handle_input(int event_type) {
    if (!current_page) return;

    // Let the page handle input (page may access its state)
    if (current_page->handle_input) {
        current_page->handle_input(event_type);
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