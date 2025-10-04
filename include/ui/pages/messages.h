/**
 * @file messages.h
 * @brief Messages conversation page
 * @ingroup ui_pages
 *
 * Provides the messages interface for viewing SMS conversations.
 * Displays message history and provides options for replying and composing new messages.
 */

#ifndef MESSAGES_H
#define MESSAGES_H

#include "screen.h"
#include "display.h"
#include "tile.h"
#include "theme.h"
#include "bottom_bar.h"
#include "sms_types.h"
#include "new_sms.h"
#include <stdlib.h>
#include <string.h>

/** @ingroup ui_pages
 *  @brief Maximum length for message text display */
#define MAX_MESSAGE_LENGTH 255

/**
 * @brief Message page state structure
 * @ingroup ui_pages
 *
 * Contains the state information for a message conversation page,
 * including the sender and message content.
 */
typedef struct
{
    char sender[SMS_MAX_PHONE_LENGTH + 1]; /**< Phone number of the message sender */
    char message[MAX_MESSAGE_LENGTH];      /**< Message text content */
} MessagePageState;

/**
 * @ingroup ui_pages
 * @brief Create a messages page
 * @param state Message page state containing sender and message data
 * @return Pointer to the messages page structure
 *
 * Creates and initializes a new messages page for displaying a conversation
 * with the specified sender and message content.
 */
Page *messages_page_create(MessagePageState state);

#endif
