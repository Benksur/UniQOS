/**
 * @file clock.h
 * @brief Clock application page
 * @ingroup ui_pages
 *
 * Provides the clock application interface displaying current time and date.
 * Shows both digital and analog clock representations.
 */

#ifndef CLOCK_H
#define CLOCK_H

#include "screen.h"

/**
 * @ingroup ui_pages
 * @brief Create the clock page
 * @return Pointer to the clock page structure
 *
 * Creates and initializes a new clock page displaying the current time
 * and date with automatic updates.
 */
Page *clock_page_create();

#endif