/**
 * @file sms.h
 * @brief SMS main page
 * @ingroup ui_pages
 *
 * Provides the main SMS interface for viewing message lists, conversations,
 * and managing SMS functionality.
 */

#ifndef SMS_H
#define SMS_H

#include "screen.h"
#include "cursor.h"
#include "display.h"
#include "tile.h"
#include "menu_row.h"
#include "input.h"
#include "theme.h"
#include "new_sms.h"
#include "messages.h"
#include <stdlib.h>
#include <string.h>

/**
 * @ingroup ui_pages
 * @brief Create the SMS page
 * @return Pointer to the SMS page structure
 *
 * Creates and initializes a new SMS page displaying the message
 * list and providing access to SMS functions.
 */
Page *sms_page_create();

#endif