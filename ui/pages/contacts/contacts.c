#include "contacts.h"
#include "memwrap.h"

typedef struct
{
    Cursor cursor;
    ContactsView view;
    int page_offset;
    bool mounted;
} ContactsState;

static void contacts_draw_tile(Page *self, int tx, int ty);
static void contacts_handle_input(Page *self, int event_type);
static void contacts_reset(Page *self);
static void contacts_destroy(Page *self);

static char *names[] = {
    "Ty Behnke",
    "James Wood",
    "Caiyan Jin",
    "Pauline Pounds",
    "Alice Smith",
    "Bob Johnson",
    "Charlie Brown",
    "Diana Prince",
    "Ethan Hunt",
    "Fiona Gallagher",
    "George Martin",
    "Hannah Baker",
    "Ian Fleming",
    "Jack Sparrow",
    "Kara Thrace",
    "Liam Neeson",
};

static void mark_row_clean(Page *self, int row)
{
    int tile_y = row;
    for (int i = 0; i < TILE_COLS; i++)
    {
        mark_tile_clean(i, tile_y);
    }
}

static void mark_row_dirty(Page *self, int row)
{
    int tile_y = row;
    for (int i = 0; i < TILE_COLS; i++)
    {
        mark_tile_dirty(i, tile_y);
    }
}

static void update_page_offset(ContactsState *state)
{
    if (state->cursor.y < state->page_offset)
    {
        state->page_offset = 0;
        mark_all_tiles_dirty();
    }
    else if (state->cursor.y >= state->page_offset + CONTACTS_VISIBLE_COUNT)
    {
        state->page_offset = state->cursor.y;
        mark_all_tiles_dirty();
    }
}

static void contacts_draw_tile(Page *self, int tx, int ty)
{
    ContactsState *state = (ContactsState *)self->state;
    if (!state->mounted)
    {
        display_fill_rect(0, 25 + TILE_HEIGHT * TILE_ROWS, TILE_WIDTH * TILE_COLS, TILE_HEIGHT, current_theme.fg_colour);
        state->mounted = true;
    }
    int item_index = state->page_offset + ty;
    if (item_index >= sizeof(names) / sizeof(names[0]))
    {
        draw_contact_row(ty, false, ""); // empty row
        mark_row_clean(self, ty);
        return;
    }
    bool highlight = (state->cursor.y == item_index);
    draw_contact_row(ty, highlight, names[item_index]);
    mark_row_clean(self, ty);
}

static void contacts_handle_input(Page *self, int event_type)
{
    int old_x, old_y;
    bool moved = false;
    ContactsState *state = (ContactsState *)self->state;

    switch (event_type)
    {
    case INPUT_DPAD_UP:
        moved = cursor_move(&state->cursor, 0, -1, &old_x, &old_y);
        break;
    case INPUT_DPAD_DOWN:
        moved = cursor_move(&state->cursor, 0, +1, &old_x, &old_y);
        break;
    case INPUT_DPAD_RIGHT:
        if (state->cursor.y < 9)
            moved = cursor_move(&state->cursor, 0, 9 - state->cursor.y, &old_x, &old_y);
        break;
    case INPUT_DPAD_LEFT:
        if (state->cursor.y >= 9)
            moved = cursor_move(&state->cursor, 0, -state->cursor.y, &old_x, &old_y);
        break;
    }

    if (moved)
    {
        update_page_offset(state);
        int old_row = old_y - state->page_offset;
        int new_row = state->cursor.y - state->page_offset;

        if (old_row >= 0 && old_row < CONTACTS_VISIBLE_COUNT)
            mark_row_dirty(self, old_row);
        if (new_row >= 0 && new_row < CONTACTS_VISIBLE_COUNT)
            mark_row_dirty(self, new_row);
    }

    if (event_type == INPUT_SELECT)
    {
        ContactRecord contact = {
            .name_len = strlen(names[state->cursor.y]),
            .phone_len = 10,
            .phone = "1234567890",
            .offset_id = 0};
        strncpy(contact.name, names[state->cursor.y], sizeof(contact.name) - 1);
        contact.name[sizeof(contact.name) - 1] = '\0';
        Page *details_page = contact_details_page_create(contact);
        screen_push_page(details_page);
    }
    // handle input, update cursor, mark tiles dirty, etc.
}

static void contacts_reset(Page *self)
{
    ContactsState *state = (ContactsState *)self->state;
    // cursor_reset(&state->cursor);
}

static void contacts_destroy(Page *self)
{
    if (self == NULL)
        return;
    ContactsState *state = (ContactsState *)self->state;
    mem_free(state);
    mem_free(self);
}

static void contacts_data_request(int type, void *req);

static void contacts_get_page(int type, void *req)
{
    screen_request(type, req);
}

Page *contacts_page_create()
{
    Page *page = mem_malloc(sizeof(Page));
    ContactsState *state = mem_malloc(sizeof(ContactsState));
    state->cursor = (Cursor){0, 0, 0, 15, false};
    state->page_offset = 0;
    state->mounted = false;

    page->draw = NULL;
    page->draw_tile = contacts_draw_tile;
    page->handle_input = contacts_handle_input;
    page->reset = contacts_reset;
    page->destroy = contacts_destroy;
    page->state = state;

    return page;
}