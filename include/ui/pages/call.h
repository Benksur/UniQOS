/**
 * @file call.h
 * @brief Phone call interface page
 * @ingroup ui_pages
 *
 * Provides the phone call interface for making and receiving calls.
 * Displays call status, contact information, and call controls.
 */

#ifndef CALL_H
#define CALL_H

#include "screen.h"

/**
 * @ingroup ui_pages
 * @brief Create a call page
 * @param phone_number Phone number to display during the call
 * @return Pointer to the call page structure
 *
 * Creates and initializes a new call page for the specified phone number.
 * The page displays call status and provides call control options.
 */
Page *call_page_create(const char *phone_number);

#endif