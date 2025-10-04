/**
 * @file menu_icon.h
 * @brief Menu icon display component
 * @ingroup ui_components
 *
 * Provides functions for drawing icons in menu interfaces. Icons are loaded
 * from files and displayed at specified tile coordinates.
 */

#ifndef MENU_ICON_H
#define MENU_ICON_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/**
 * @ingroup ui_components
 * @brief Draw a menu icon from file
 * @param tile_x Horizontal tile position
 * @param tile_y Vertical tile position
 * @param filename Path to the icon file to display
 *
 * Loads and displays an icon from the specified file at the given tile
 * coordinates. The icon is scaled to fit within a single tile.
 */
void draw_menu_icon(int tile_x, int tile_y, const char *filename);

#endif