/**
 * @file debug.h
 * @brief Debug information page
 * @ingroup ui_pages
 *
 * Provides the debug interface for viewing system information, logs,
 * and diagnostic data. Used for troubleshooting and development.
 */

#ifndef DEBUG_H
#define DEBUG_H

#include "screen.h"

/**
 * @ingroup ui_pages
 * @brief Create the debug page
 * @return Pointer to the debug page structure
 *
 * Creates and initializes a new debug page displaying system
 * information, memory usage, and diagnostic data.
 */
Page *debug_page_create();

#endif
