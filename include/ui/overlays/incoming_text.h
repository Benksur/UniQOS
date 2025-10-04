/**
 * @file incoming_text.h
 * @brief Incoming text message notification overlay
 * @ingroup ui_overlays
 *
 * Provides the incoming text message notification overlay that appears when
 * a new SMS is received. Displays sender information and provides open/dismiss options.
 */

#ifndef INCOMING_TEXT_H
#define INCOMING_TEXT_H

#include "screen.h"
#include "input.h"
#include "cursor.h"
#include "theme.h"
#include "display.h"
#include "tile.h"
#include <string.h>
#include <stdlib.h>

/**
 * @brief Callback function type for incoming text actions
 * @ingroup ui_overlays
 * @param action Action taken by the user (INCOMING_TEXT_ACTION_OPEN or INCOMING_TEXT_ACTION_CLOSE)
 * @param user_data User-provided data pointer
 */
typedef void (*IncomingTextCallback)(int action, void *user_data);

/** @ingroup ui_overlays
 *  @brief Action constant for opening the text message */
#define INCOMING_TEXT_ACTION_OPEN 0

/** @ingroup ui_overlays
 *  @brief Action constant for dismissing the notification */
#define INCOMING_TEXT_ACTION_CLOSE 1

/**
 * @ingroup ui_overlays
 * @brief Create an incoming text overlay
 * @param sender_number Phone number of the message sender
 * @param callback Callback function to handle user actions
 * @param user_data User data to pass to the callback
 * @return Pointer to the incoming text overlay page
 *
 * Creates and initializes an incoming text message notification overlay
 * displaying the sender's phone number and action buttons.
 */
Page *incoming_text_overlay_create(const char *sender_number, IncomingTextCallback callback, void *user_data);

#endif // INCOMING_TEXT_H
