#ifndef SCREEN_H
#define SCREEN_H


typedef struct {
    void (*draw)(struct Page* self);
    void (*draw_tile)(int tx, int ty, struct Page* self);
    void (*handle_input)(int event_type, struct Page* self);
    void (*reset)(struct Page* self);
    void (*destroy)(struct Page* self);
    void* state;
} Page;

void screen_init(Page* initial_page);
void screen_push_page(Page* new_page);
void screen_pop_page(void);
void screen_set_page(Page* new_page);
void screen_handle_input(int event_type);
void screen_tick(void);

#endif
