/**
 * @file sweeper.h
 * @brief Minesweeper game page
 * @ingroup ui_pages
 *
 * Provides the Minesweeper game interface. Players must clear a minefield
 * by revealing safe squares while avoiding mines.
 */

#ifndef GSWEEPER_H
#define GSWEEPER_H

#include "screen.h"

/**
 * @ingroup ui_pages
 * @brief Create the Minesweeper game page
 * @return Pointer to the Minesweeper game page structure
 *
 * Creates and initializes a new Minesweeper game page with a
 * randomly generated minefield.
 */
Page *sweeper_page_create();

#endif