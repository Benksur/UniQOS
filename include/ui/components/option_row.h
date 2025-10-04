/**
 * @file option_row.h
 * @brief Option row component for settings and menus
 * @ingroup ui_components
 *
 * Provides functions for drawing option rows in settings menus and similar
 * interfaces. Supports both normal and highlighted background variants.
 */

#ifndef OPTION_ROW_H
#define OPTION_ROW_H

#include "tile.h"
#include "theme.h"
#include "display.h"
#include <string.h>

/**
 * @ingroup ui_components
 * @brief Draw an option row
 * @param tile_y Vertical tile position for the row
 * @param selected Non-zero if this row is currently selected
 * @param label Text label to display for the option
 *
 * Draws a single option row with the specified label. The row will be
 * highlighted if selected is non-zero.
 */
void draw_option_row(int tile_y, int selected, const char *label);

/**
 * @ingroup ui_components
 * @brief Draw an option row with highlighted background
 * @param tile_y Vertical tile position for the row
 * @param selected Non-zero if this row is currently selected
 * @param label Text label to display for the option
 *
 * Draws a single option row with the specified label and a highlighted
 * background. This variant provides more visual emphasis for important options.
 */
void draw_option_row_highlight_bg(int tile_y, int selected, const char *label);

#endif