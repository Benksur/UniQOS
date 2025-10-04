/**
 * @file screen.h
 * @brief Screen management and page system
 * @ingroup ui_screen
 *
 * Manages the screen stack and page lifecycle. Provides a page-based navigation
 * system where each page represents a full-screen application view.
 */

#ifndef SCREEN_H
#define SCREEN_H

#include <stdbool.h>

/**
 * @brief Page data response types
 * @ingroup ui_screen
 */
typedef enum
{
    PAGE_RESPONSE_DIALLING,    /**< Call dialing response */
    PAGE_RESPONSE_ACTIVE_CALL, /**< Active call response */
    PAGE_RESPONSE_CALL_ENDED,  /**< Call ended response */
    PAGE_RESPONSE_BATTERY_HC   /**< Battery health check response */
} PageDataResponse;

/**
 * @brief Page data request types
 * @ingroup ui_screen
 */
typedef enum
{
    PAGE_REQUEST_HANGUP_CALL, /**< Request to hang up call */
    PAGE_REQUEST_MAKE_CALL,   /**< Request to make a call */
    PAGE_REQUEST_SMS_SEND,    /**< Request to send SMS */
    PAGE_REQUEST_BATTERY_HC   /**< Request battery health check */
} PageDataRequest;

typedef struct Page Page; // Forward declaration

/**
 * @brief Page structure with virtual function table
 * @ingroup ui_screen
 *
 * Each page must implement these function pointers to handle
 * drawing, input, and lifecycle events.
 */
typedef struct Page
{
    void (*draw)(Page *self);                                /**< Draw the entire page */
    void (*draw_tile)(Page *self, int tx, int ty);           /**< Draw a specific tile */
    void (*handle_input)(Page *self, int event_type);        /**< Handle input event */
    void (*reset)(Page *self);                               /**< Reset page state */
    void (*destroy)(Page *self);                             /**< Clean up page resources */
    void (*data_response)(Page *self, int type, void *resp); /**< Handle data response */
    void *state;                                             /**< Page-specific state data */
} Page;

/**
 * @ingroup ui_screen
 * @brief Initialize the screen system
 * @param initial_page Initial page to display
 */
void screen_init(Page *initial_page);

/**
 * @ingroup ui_screen
 * @brief Get the current active page
 * @return Pointer to current page
 */
Page *screen_get_current_page(void);

/**
 * @ingroup ui_screen
 * @brief Push a new page onto the stack
 * @param new_page Page to push
 *
 * The new page becomes active, but previous page is preserved
 * and can be returned to with screen_pop_page().
 */
void screen_push_page(Page *new_page);

/**
 * @ingroup ui_screen
 * @brief Pop the current page from the stack
 *
 * Returns to the previous page. Does nothing if only one page remains.
 */
void screen_pop_page(void);

/**
 * @ingroup ui_screen
 * @brief Replace the current page
 * @param new_page Page to set as current
 *
 * Destroys the current page and replaces it with the new page.
 */
void screen_set_page(Page *new_page);

/**
 * @ingroup ui_screen
 * @brief Handle input event for current page
 * @param event_type Input event type
 */
void screen_handle_input(int event_type);

/**
 * @ingroup ui_screen
 * @brief Periodic tick for animations and updates
 *
 * Should be called regularly to allow pages to update state.
 */
void screen_tick(void);

/**
 * @ingroup ui_screen
 * @brief Send a request from a page
 * @param type Request type
 * @param req Request data
 */
void screen_request(int type, void *req);

/**
 * @ingroup ui_screen
 * @brief Check for pending page request
 * @param type Pointer to receive request type
 * @param req Pointer to receive request data
 * @return true if a request is pending
 */
bool screen_get_pending_request(int *type, void **req);

/**
 * @ingroup ui_screen
 * @brief Handle a data response for current page
 * @param type Response type
 * @param resp Response data
 */
void screen_handle_response(int type, void *resp);

#endif /* SCREEN_H */
