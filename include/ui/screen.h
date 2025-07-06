#ifndef SCREEN_H
#define SCREEN_H


typedef struct {
    void (*draw)(void);
    void (*draw_tile)(int tile_x, int tile_y);
    void (*handle_input)(int event_type, int x, int y);
} Page;

void screen_init(Page* initial_page);
void screen_push_page(Page* new_page);
void screen_pop_page(void); 
void screen_set_page(Page* new_page);
void screen_handle_input(int event_type, int x, int y);
void screen_tick(void);

#endif
