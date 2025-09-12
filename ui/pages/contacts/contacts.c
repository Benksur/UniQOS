#include "contacts.h"

typedef struct {
    Cursor cursor;
    ContactsView view;
    int page_offset;
    bool mounted;
} ContactsState;

static void contacts_draw(Page* self);
static void contacts_draw_tile(Page* self, int tx, int ty);
static void contacts_handle_input(Page* self, int event_type);
static void contacts_reset(Page* self);
static void contacts_destroy(Page* self);

static void contacts_draw(Page* self) {
    ContactsState* state = (ContactsState*)self->state;
    for (int ty = 0; ty < state->view.visible_count; ty++) {
        // draw_contact_row(ty, ty == state->cursor.y, state->names[ty]);
    }
}

static void contacts_draw_tile(Page* self, int tx, int ty) {
    ContactsState* state = (ContactsState*)self->state;
    // draw_contact_row(ty, ty == state->cursor.y, state->names[ty]);
}

static void contacts_handle_input(Page* self, int event_type) {
    ContactsState* state = (ContactsState*)self->state;
    // handle input, update cursor, mark tiles dirty, etc.
}

static void contacts_reset(Page* self) {
    ContactsState* state = (ContactsState*)self->state;
    cursor_reset(&state->cursor);
}

static void contacts_destroy(Page* self) {
    if (self == NULL) return;
    ContactsState* state = (ContactsState*)self->state;
    free(state);
    free(self);
}

static void contacts_get_page(int type, void* req) {
    screen_data_request(type, req);
}

Page* contacts_page_create() {
    Page* page = malloc(sizeof(Page));
    ContactsState* state = malloc(sizeof(ContactsState));
    state->cursor = (Cursor){0, 0, 0, CONTACTS_VISIBLE_COUNT - 1, false};
    state->mounted = false;


    page->draw = contacts_draw;
    page->draw_tile = contacts_draw_tile;
    page->handle_input = contacts_handle_input;
    page->reset = contacts_reset;
    page->destroy = contacts_destroy;
    page->state = state;

    return page;
}