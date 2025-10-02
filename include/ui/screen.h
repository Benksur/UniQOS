#ifndef SCREEN_H
#define SCREEN_H

#include <stdbool.h>

typedef enum
{
    PAGE_RESPONSE_DIALLING,
    PAGE_RESPONSE_ACTIVE_CALL,
    PAGE_RESPONSE_CALL_ENDED,
} PageDataResponse;

typedef enum
{
    PAGE_REQUEST_HANGUP_CALL,
    PAGE_REQUEST_MAKE_CALL,
    PAGE_REQUEST_SMS_SEND
} PageDataRequest;

typedef struct Page Page; // Forward declaration

typedef struct Page
{
    void (*draw)(Page *self);
    void (*draw_tile)(Page *self, int tx, int ty);
    void (*handle_input)(Page *self, int event_type);
    void (*reset)(Page *self);
    void (*destroy)(Page *self);
    void (*data_response)(Page *self, int type, void *resp);
    void *state;
} Page;

void screen_init(Page *initial_page);
Page *screen_get_current_page(void);
void screen_push_page(Page *new_page);
void screen_pop_page(void);
void screen_set_page(Page *new_page);
void screen_handle_input(int event_type);
void screen_tick(void);
void screen_request(int type, void *req);
bool screen_get_pending_request(int *type, void **req);
void screen_handle_response(int type, void *resp);

#endif
