#include "contacts.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tile.h"
#include "cursor.h"

#define CONTACTS_VISIBLE_COUNT 8

typedef struct {
    Cursor cursor;
    char* contact_names[CONTACTS_VISIBLE_COUNT];
    int count;
    bool mounted;
} ContactsState;

static void contacts_draw();
static void contacts_draw_tile(int tx, int ty);
static void contacts_handle_input(int event_type);
static void contacts_reset();
static void contacts_destroy();

static Page* current_page = NULL; // Used only for static draw helpers

static void contacts_draw() {
    ContactsState* state = (ContactsState*)current_page->state;
    for (int ty = 0; ty < state->count; ty++) {
        // draw_contact_row(ty, ty == state->cursor.y, state->names[ty]);
    }
}

static void contacts_draw_tile(int tx, int ty) {
    ContactsState* state = (ContactsState*)current_page->state;
    // draw_contact_row(ty, ty == state->cursor.y, state->names[ty]);
}

static void contacts_handle_input(int event_type) {
    ContactsState* state = (ContactsState*)current_page->state;
    // handle input, update cursor, mark tiles dirty, etc.
}

static void contacts_reset() {
    ContactsState* state = (ContactsState*)current_page->state;
    cursor_reset(&state->cursor);
}

static void contacts_destroy() {
    ContactsState* state = (ContactsState*)current_page->state;
    free(state->names);
    free(state);
    free(current_page);
}

Page* contacts_page_create(const char* const* names, int count) {
    Page* page = malloc(sizeof(Page));
    ContactsState* state = malloc(sizeof(ContactsState));
    state->cursor = (Cursor){0, 0, 0, count - 1, false};
    state->count = count;
    state->names = malloc(sizeof(char*) * count);
    for (int i = 0; i < count; ++i) {
        state->names[i] = strdup(names[i]);
    }

    page->draw = contacts_draw;
    page->draw_tile = contacts_draw_tile;
    page->handle_input = contacts_handle_input;
    page->reset = contacts_reset;
    page->destroy = contacts_destroy;
    page->state = state;

    current_page = page; // Set for static helpers
    return page;
}