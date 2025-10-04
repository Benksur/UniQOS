/**
 * @file idisplay_driver.h
 * @brief Display driver interface definitions
 * @ingroup display_drivers
 *
 * Defines the abstract interface for display drivers. This allows the system
 * to work with different display controllers through a common interface.
 */

#pragma once
#include <stdint.h>

/**
 * @brief Display driver interface structure
 * @ingroup display_drivers
 *
 * Defines the complete interface for display driver implementations.
 * All display drivers must implement this interface.
 */
typedef struct
{
    void (*init)(void); /**< Initialize the display driver */

    void (*fill)(uint16_t colour); /**< Fill entire screen with color */

    void (*fill_rect)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t colour); /**< Fill rectangle with color */

    void (*draw_pixel)(uint16_t x, uint16_t y, uint16_t colour); /**< Draw a single pixel */

    void (*draw_hline)(uint16_t x, uint16_t y, uint16_t length, uint16_t colour); /**< Draw horizontal line */

    void (*draw_vline)(uint16_t x, uint16_t y, uint16_t length, uint16_t colour); /**< Draw vertical line */

    void (*set_orientation)(uint8_t rotation); /**< Set display orientation (0-3) */

    uint16_t (*get_width)(void);  /**< Get display width in pixels */
    uint16_t (*get_height)(void); /**< Get display height in pixels */

    void (*draw_bitmap)(uint16_t x, uint16_t y, const uint8_t *bitmap, uint16_t width, uint16_t height, uint16_t fg_colour, uint16_t bg_colour); /**< Draw bitmap image */
} IDisplayDriver_t;