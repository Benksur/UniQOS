/**
 * @file menu_row.h
 * @brief Menu row display component
 * @ingroup ui_components
 *
 * Provides functions for drawing individual rows in menu interfaces.
 * Supports text rows with selection highlighting and empty rows for spacing.
 */

#ifndef MENU_ROW_H
#define MENU_ROW_H

#include <stdint.h>

/**
 * @ingroup ui_components
 * @brief Draw a menu row with text
 * @param tile_y Vertical tile position for the row
 * @param selected Non-zero if this row is currently selected
 * @param text Text to display in the row
 *
 * Draws a single row in a menu with the specified text. The row will be
 * highlighted if selected is non-zero.
 */
void draw_menu_row(int tile_y, int selected, const char *text);

/**
 * @ingroup ui_components
 * @brief Draw an empty row for spacing
 * @param tile_y Vertical tile position for the row
 *
 * Draws an empty row at the specified position, typically used for
 * spacing between menu sections.
 */
void draw_empty_row(int tile_y);

/**
 * @ingroup ui_components
 * @brief Draw an empty row with background color
 * @param tile_y Vertical tile position for the row
 * @param colour Background color for the row (16-bit RGB565 format)
 *
 * Draws an empty row at the specified position with the given background
 * color, useful for creating colored dividers or backgrounds.
 */
void draw_empty_row_fill(int tile_y, uint16_t colour);

#endif
