/**
 * @file contact_row.h
 * @brief Contact list row component
 * @ingroup ui_components
 *
 * Provides functions for drawing individual contact entries in a contact list.
 * Supports selection highlighting and proper text formatting.
 */

#ifndef CONTACT_ROW_H
#define CONTACT_ROW_H

#include "tile.h"
#include "theme.h"
#include "display.h"
#include <string.h>

/**
 * @ingroup ui_components
 * @brief Draw a contact list row
 * @param tile_y Vertical tile position for the row
 * @param selected Non-zero if this row is currently selected
 * @param name Contact name to display
 *
 * Draws a single row in a contact list with the specified contact name.
 * The row will be highlighted if selected is non-zero.
 */
void draw_contact_row(int tile_y, int selected, const char *name);

#endif