/**
 * @file new_sms.h
 * @brief New SMS composition page
 * @ingroup ui_pages
 *
 * Provides the interface for composing and sending new SMS messages.
 * Includes text input, recipient selection, and message sending functionality.
 */

#ifndef NEW_SMS_H
#define NEW_SMS_H

#include "screen.h"
#include "display.h"
#include "tile.h"
#include "menu_row.h"
#include "theme.h"
#include "input.h"
#include "cursor.h"
#include <stdlib.h>
#include <string.h>

/**
 * @ingroup ui_pages
 * @brief Create a new SMS page
 * @param phone_number Phone number to pre-fill as recipient
 * @return Pointer to the new SMS page structure
 *
 * Creates and initializes a new SMS composition page with the specified
 * recipient phone number pre-filled.
 */
Page *new_sms_page_create(const char *phone_number);

#endif