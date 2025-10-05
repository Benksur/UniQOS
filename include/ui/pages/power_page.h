/**
 * @file power.h
 * @brief Power management page
 * @ingroup ui_pages
 *
 * Provides the power management interface for viewing battery status,
 * power settings, and system power information.
 */

#ifndef POWERP_H
#define POWERP_H

#include "screen.h"

/**
 * @ingroup ui_pages
 * @brief Create the power page
 * @return Pointer to the power page structure
 *
 * Creates and initializes a new power page displaying battery
 * status and power management options.
 */
Page *power_page_create();

#endif