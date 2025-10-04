/**
 * @file incoming_call.h
 * @brief Incoming call notification overlay
 * @ingroup ui_overlays
 *
 * Provides the incoming call notification overlay that appears when a call
 * is received. Displays caller information and provides accept/decline options.
 */

#ifndef INCOMING_CALL_H
#define INCOMING_CALL_H

#include "screen.h"
#include "input.h"
#include "call.h"
#include "cursor.h"
#include "theme.h"
#include "display.h"
#include "tile.h"

#include <string.h>
#include <stdlib.h>

/**
 * @brief Callback function type for incoming call actions
 * @ingroup ui_overlays
 * @param action Action taken by the user (INCOMING_CALL_ACTION_PICKUP or INCOMING_CALL_ACTION_HANGUP)
 * @param user_data User-provided data pointer
 */
typedef void (*IncomingCallCallback)(int action, void *user_data);

/** @ingroup ui_overlays
 *  @brief Action constant for picking up the call */
#define INCOMING_CALL_ACTION_PICKUP 0

/** @ingroup ui_overlays
 *  @brief Action constant for hanging up the call */
#define INCOMING_CALL_ACTION_HANGUP 1

/**
 * @ingroup ui_overlays
 * @brief Create an incoming call overlay
 * @param phone_number Phone number of the incoming caller
 * @param callback Callback function to handle user actions
 * @param user_data User data to pass to the callback
 * @return Pointer to the incoming call overlay page
 *
 * Creates and initializes an incoming call notification overlay
 * displaying the caller's phone number and action buttons.
 */
Page *incoming_call_overlay_create(const char *phone_number, IncomingCallCallback callback, void *user_data);

#endif // INCOMING_CALL_H
