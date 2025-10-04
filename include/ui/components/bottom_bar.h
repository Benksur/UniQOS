/**
 * @file bottom_bar.h
 * @brief Bottom navigation bar component
 * @ingroup ui_components
 *
 * Provides functions for drawing a bottom navigation bar with left, center,
 * and right text elements. The bar uses the current theme colors and supports
 * accent highlighting.
 */

#ifndef BOTTOM_BAR_H
#define BOTTOM_BAR_H

#include "tile.h"
#include "theme.h"
#include "display.h"
#include <string.h>

/**
 * @ingroup ui_components
 * @brief Draw a bottom navigation bar
 * @param left Text to display on the left side of the bar
 * @param center Text to display in the center of the bar
 * @param right Text to display on the right side of the bar
 * @param accent_index Color index for accent highlighting (0-7)
 *
 * Draws a horizontal bar at the bottom of the screen with three text elements.
 * The bar uses the current theme colors and applies accent highlighting based
 * on the accent_index parameter.
 */
void draw_bottom_bar(const char *left, const char *center, const char *right, int accent_index);

#endif