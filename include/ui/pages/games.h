/**
 * @file games.h
 * @brief Games menu page
 * @ingroup ui_pages
 *
 * Provides the games menu interface for accessing available games.
 * Displays a list of games and allows users to launch them.
 */

#ifndef GAMES_H
#define GAMES_H

#include "screen.h"

/**
 * @ingroup ui_pages
 * @brief Create the games page
 * @return Pointer to the games page structure
 *
 * Creates and initializes a new games page displaying available
 * games in a menu format for user selection.
 */
Page *games_page_create();

#endif
