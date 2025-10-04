/**
 * @file phone.h
 * @brief Phone application page
 * @ingroup ui_pages
 *
 * Provides the phone application interface for making calls, viewing
 * call history, and managing phone-related functions.
 */

#ifndef PHONE_H
#define PHONE_H

#include "screen.h"

/**
 * @ingroup ui_pages
 * @brief Create the phone page
 * @return Pointer to the phone page structure
 *
 * Creates and initializes a new phone page with dialer interface
 * and call history access.
 */
Page *phone_page_create();

#endif