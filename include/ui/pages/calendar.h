/**
 * @file calendar.h
 * @brief Calendar application page
 * @ingroup ui_pages
 *
 * Provides the calendar application interface for viewing dates and events.
 * Displays a monthly calendar view with navigation capabilities.
 */

#ifndef CALENDAR_H
#define CALENDAR_H

#include "screen.h"

/**
 * @ingroup ui_pages
 * @brief Create the calendar page
 * @return Pointer to the calendar page structure
 *
 * Creates and initializes a new calendar page showing the current month
 * with today's date highlighted.
 */
Page *calendar_page_create();

#endif
