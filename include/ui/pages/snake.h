/**
 * @file snake.h
 * @brief Snake game page
 * @ingroup ui_pages
 *
 * Provides the Snake game interface. Players control a snake that grows
 * by eating food while avoiding walls and the snake's own body.
 */

#ifndef GNSAKE_H
#define GSNAKE_H

#include "screen.h"

/**
 * @ingroup ui_pages
 * @brief Create the Snake game page
 * @return Pointer to the Snake game page structure
 *
 * Creates and initializes a new Snake game page with a fresh game
 * board and initial snake position.
 */
Page *snake_page_create();

#endif