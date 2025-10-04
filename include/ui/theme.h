/**
 * @file theme.h
 * @brief UI theme and color management
 * @ingroup ui_theme
 *
 * Provides color scheme management for the user interface.
 * Supports light and dark themes with consistent color palettes.
 */

#ifndef THEME_H
#define THEME_H

#include <stdint.h>

/**
 * @brief Theme color structure
 * @ingroup ui_theme
 *
 * Defines a complete color scheme for UI elements.
 */
typedef struct
{
    uint16_t bg_colour;        /**< Background color (RGB565) */
    uint16_t text_colour;      /**< Primary text color (RGB565) */
    uint16_t fg_colour;        /**< Foreground color (RGB565) */
    uint16_t accent_colour;    /**< Accent color for highlights (RGB565) */
    uint16_t highlight_colour; /**< Highlight color for selection (RGB565) */
} Theme;

/** @ingroup ui_theme
 *  @brief Currently active theme */
extern Theme current_theme;

/**
 * @ingroup ui_theme
 * @brief Set light theme colors
 *
 * Applies a light color scheme with dark text on light backgrounds.
 */
void theme_set_light(void);

/**
 * @ingroup ui_theme
 * @brief Set dark theme colors
 *
 * Applies a dark color scheme with light text on dark backgrounds.
 */
void theme_set_dark(void);

#endif
