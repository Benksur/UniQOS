#ifndef SCREEN_H
#define SCREEN_H

#include <stdbool.h>

typedef void (*DataRequestFn)(int type, void* req);
typedef void (*DataResponseFn)(int type, void* resp);

typedef struct {
    void (*draw)();
    void (*draw_tile)(int tx, int ty);
    void (*handle_input)(int event_type);
    void (*reset)();
    void (*destroy)();
    bool loading;
    DataRequestFn data_request;
    DataResponseFn data_response;
    void* state;
} Page;

void screen_set_data_req_fn(DataRequestFn fn);
void screen_set_data_resp_fn(DataResponseFn fn);
void screen_init(Page* initial_page);
void screen_push_page(Page* new_page);
void screen_pop_page(void);
void screen_set_page(Page* new_page);
void screen_handle_input(int event_type);
void screen_tick(void);

#endif
