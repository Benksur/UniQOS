#include "screen.h"
#include "tile.h"
#include "status_bar.h"
#include <stdlib.h>
#include <stdbool.h>

#define MAX_PAGE_STACK 10

static Page *page_stack[MAX_PAGE_STACK];
static int page_top = -1;
static Page *current_page = NULL;

// Internal state for pending requests
static bool has_pending_request = false;
static int pending_request_type = 0;
static void *pending_request_data = NULL;

/**
 * Initialize the screen with the initial page.
 */
void screen_init(Page *initial_page)
{
    page_top = -1;
    current_page = initial_page;

    if (current_page)
    {
        if (current_page->reset)
            current_page->reset(current_page);
        current_page->draw(current_page);
    }
}

/**
 * Push a new page onto the stack.
 */
void screen_push_page(Page *new_page)
{
    if (page_top < MAX_PAGE_STACK - 1)
    {

        page_stack[++page_top] = current_page;

        current_page = new_page;

        if (current_page->reset)
            current_page->reset(current_page);
        mark_all_tiles_dirty();
        if (current_page->draw)
            current_page->draw(current_page);
    }
}

/**
 * Pop the current page off the stack.
 */
void screen_pop_page(void)
{
    if (page_top >= 0)
    {
        // Free current page if dynamic
        if (current_page && current_page->destroy)
        {
            current_page->destroy(current_page);
        }

        // Restore the previous page from the stack
        current_page = page_stack[page_top--];

        if (current_page->reset && current_page)
            current_page->reset(current_page);
        mark_all_tiles_dirty();
        if (current_page->draw)
            current_page->draw(current_page);
    }
}

/**
 * Replace the current page with a new page.
 */
void screen_set_page(Page *new_page)
{
    // Free current page if dynamic
    if (current_page && current_page->destroy)
    {
        current_page->destroy(current_page);
    }

    current_page = new_page;

    if (current_page && current_page->reset)
    {
        current_page->reset(current_page);
    }
    mark_all_tiles_dirty();
    if (current_page && current_page->draw)
    {
        current_page->draw(current_page);
    }
}

/**
 * Handle input for the current page.
 */
void screen_handle_input(int event_type)
{
    if (!current_page)
        return;

    // Let the page handle input (page may access its state)
    if (current_page->handle_input)
    {
        current_page->handle_input(current_page, event_type);
    }
}

void screen_request(int type, void *req)
{
    // Store the request for display_task to handle
    // Only store one request at a time (latest request wins)
    has_pending_request = true;
    pending_request_type = type;
    pending_request_data = req;
}

bool screen_get_pending_request(int *type, void **req)
{
    if (!has_pending_request)
        return false;

    if (type)
        *type = pending_request_type;
    if (req)
        *req = pending_request_data;

    // Clear the pending request
    has_pending_request = false;
    pending_request_type = 0;
    pending_request_data = NULL;

    return true;
}

void screen_handle_response(int type, void *resp)
{
    if (!current_page)
        return;
    if (current_page->data_response)
    {
        current_page->data_response(current_page, type, resp);
    }
}

/**
 * Flush all dirty tiles.
 */
void screen_tick(void)
{
    if (!current_page)
        return;
    // check response buffer and call screen handle data response
    if (current_page->draw_tile)
    {
        flush_dirty_tiles(current_page);
    }
    status_bar_tick();
}