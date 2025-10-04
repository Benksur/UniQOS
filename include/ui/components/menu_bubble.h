/**
 * @file menu_bubble.h
 * @brief Menu bubble background component
 * @ingroup ui_components
 *
 * Provides functions for drawing circular bubble backgrounds used in menu
 * interfaces. Supports both normal and selected states with different styling.
 */

#ifndef MENU_BUBBLE_H
#define MENU_BUBBLE_H

#include <stdbool.h>

/**
 * @ingroup ui_components
 * @brief Draw a normal menu bubble
 * @param tile_x Horizontal tile position
 * @param tile_y Vertical tile position
 *
 * Draws a circular bubble background at the specified tile coordinates
 * using the normal (unselected) styling.
 */
void draw_menu_bubble(int tile_x, int tile_y);

/**
 * @ingroup ui_components
 * @brief Draw a selected menu bubble
 * @param tile_x Horizontal tile position
 * @param tile_y Vertical tile position
 *
 * Draws a circular bubble background at the specified tile coordinates
 * using the selected (highlighted) styling to indicate the current selection.
 */
void draw_selected_menu_bubble(int tile_x, int tile_y);

#endif
