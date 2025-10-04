/**
 * @file display.h
 * @brief Display driver header
 * @ingroup display_driver
 *
 * High-level display functions for drawing graphics, shapes, and text.
 * This module provides a convenient API for rendering to the LCD display.
 */
#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "st7789v.h"

#define COLOUR_BLACK 0x0000
#define COLOUR_WHITE 0xFFFF
#define COLOUR_RED 0xF800
#define COLOUR_GREEN 0x07E0
#define COLOUR_BLUE 0x001F
#define COLOUR_YELLOW 0xFFE0
#define COLOUR_CYAN 0x07FF
#define COLOUR_MAGENTA 0xF81F
#define COLOUR_GRAY 0x7BEF
#define COLOUR_ORANGE 0xFC20

/**
 * @brief Point structure for 2D coordinates
 * @ingroup display_driver
 */
typedef struct
{
    uint16_t x; /**< X coordinate */
    uint16_t y; /**< Y coordinate */
} point_t;

/**
 * @brief Rectangle structure
 * @ingroup display_driver
 */
typedef struct
{
    uint16_t x;      /**< Top-left X coordinate */
    uint16_t y;      /**< Top-left Y coordinate */
    uint16_t width;  /**< Rectangle width */
    uint16_t height; /**< Rectangle height */
} rect_t;

/**
 * @ingroup display_driver
 * @brief Initialize the display driver
 */
void display_init(void);

/**
 * @ingroup display_driver
 * @brief Fill entire screen with a color
 * @param colour RGB565 color value
 */
void display_fill(uint16_t colour);

/**
 * @ingroup display_driver
 * @brief Fill a rectangular region
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 * @param width Rectangle width in pixels
 * @param height Rectangle height in pixels
 * @param colour RGB565 color value
 */
void display_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t colour);

/**
 * @ingroup display_driver
 * @brief Draw a rectangle outline
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 * @param width Rectangle width in pixels
 * @param height Rectangle height in pixels
 * @param colour RGB565 color value
 */
void display_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t colour);

/**
 * @ingroup display_driver
 * @brief Draw a vertical line
 * @param x X coordinate
 * @param y0 Starting Y coordinate
 * @param y1 Ending Y coordinate
 * @param colour RGB565 color value
 */
void display_draw_vertical_line(uint16_t x, uint16_t y0, uint16_t y1, uint16_t colour);

/**
 * @ingroup display_driver
 * @brief Draw a horizontal line
 * @param x0 Starting X coordinate
 * @param y Y coordinate
 * @param x1 Ending X coordinate
 * @param colour RGB565 color value
 */
void display_draw_horizontal_line(uint16_t x0, uint16_t y, uint16_t x1, uint16_t colour);

/**
 * @ingroup display_driver
 * @brief Draw a line between two points
 * @param x0 Starting X coordinate
 * @param y0 Starting Y coordinate
 * @param x1 Ending X coordinate
 * @param y1 Ending Y coordinate
 * @param colour RGB565 color value
 */
void display_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t colour);

/**
 * @ingroup display_driver
 * @brief Draw a circle outline
 * @param x0 Center X coordinate
 * @param y0 Center Y coordinate
 * @param radius Circle radius in pixels
 * @param colour RGB565 color value
 */
void display_draw_circle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t colour);

/**
 * @ingroup display_driver
 * @brief Draw a filled circle
 * @param x0 Center X coordinate
 * @param y0 Center Y coordinate
 * @param radius Circle radius in pixels
 * @param colour RGB565 color value
 */
void display_fill_circle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t colour);

/**
 * @ingroup display_driver
 * @brief Draw a rounded rectangle outline
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 * @param width Rectangle width in pixels
 * @param height Rectangle height in pixels
 * @param radius Corner radius in pixels
 * @param colour RGB565 color value
 */
void display_draw_rounded_square(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t radius, uint16_t colour);

/**
 * @ingroup display_driver
 * @brief Draw a filled rounded rectangle
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 * @param width Rectangle width in pixels
 * @param height Rectangle height in pixels
 * @param radius Corner radius in pixels
 * @param colour RGB565 color value
 */
void display_fill_rounded_square(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t radius, uint16_t colour);

/**
 * @ingroup display_driver
 * @brief Draw a triangle outline
 * @param x0 First vertex X coordinate
 * @param y0 First vertex Y coordinate
 * @param x1 Second vertex X coordinate
 * @param y1 Second vertex Y coordinate
 * @param x2 Third vertex X coordinate
 * @param y2 Third vertex Y coordinate
 * @param colour RGB565 color value
 */
void display_draw_triangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t colour);

/**
 * @ingroup display_driver
 * @brief Draw a filled triangle
 * @param x0 First vertex X coordinate
 * @param y0 First vertex Y coordinate
 * @param x1 Second vertex X coordinate
 * @param y1 Second vertex Y coordinate
 * @param x2 Third vertex X coordinate
 * @param y2 Third vertex Y coordinate
 * @param colour RGB565 color value
 */
void display_fill_triangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t colour);

/**
 * @ingroup display_driver
 * @brief Draw a single character
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 * @param c Character to draw
 * @param colour Text color in RGB565
 * @param bg_colour Background color in RGB565
 * @param size Font size multiplier
 */
void display_draw_char(uint16_t x, uint16_t y, char c, uint16_t colour, uint16_t bg_colour, uint8_t size);

/**
 * @ingroup display_driver
 * @brief Draw a string of text
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 * @param str Null-terminated string to draw
 * @param colour Text color in RGB565
 * @param bg_colour Background color in RGB565
 * @param size Font size multiplier
 */
void display_draw_string(uint16_t x, uint16_t y, const char *str, uint16_t colour, uint16_t bg_colour, uint8_t size);

/**
 * @ingroup display_driver
 * @brief Draw a bitmap from bit array
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 * @param buff Pointer to bitmap data
 * @param colour Foreground color in RGB565
 * @param bg_colour Background color in RGB565
 * @param w Bitmap width in pixels
 * @param h Bitmap height in pixels
 */
void display_draw_bits(uint16_t x, uint16_t y, uint8_t *buff, uint16_t colour, uint16_t bg_colour, uint16_t w, uint16_t h);

/**
 * @ingroup display_driver
 * @brief Set display rotation
 * @param rotation Rotation value (0-3)
 */
void display_set_rotation(uint8_t rotation);

/**
 * @ingroup display_driver
 * @brief Convert RGB888 to RGB565 color format
 * @param r Red component (0-255)
 * @param g Green component (0-255)
 * @param b Blue component (0-255)
 * @return RGB565 color value
 */
uint16_t display_colour565(uint8_t r, uint8_t g, uint8_t b);

/**
 * @ingroup display_driver
 * @brief Draw scrolling text with offset
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 * @param width Maximum width for text area
 * @param text Null-terminated string to draw
 * @param colour Text color in RGB565
 * @param bg_colour Background color in RGB565
 * @param size Font size multiplier
 * @param offset Horizontal scroll offset in pixels
 */
void display_scroll_text(uint16_t x, uint16_t y, uint16_t width, const char *text, uint16_t colour, uint16_t bg_colour, uint8_t size, int16_t offset);

/**
 * @ingroup display_driver
 * @brief Draw a progress bar
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 * @param width Progress bar width
 * @param height Progress bar height
 * @param percent Fill percentage (0-100)
 * @param fill_colour Fill color in RGB565
 * @param bg_colour Background color in RGB565
 */
void display_draw_progress_bar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t percent, uint16_t fill_colour, uint16_t bg_colour);

/**
 * @ingroup display_driver
 * @brief Draw a battery icon with charge level
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 * @param percent Battery charge level (0-100)
 * @param fg_colour Foreground color in RGB565
 * @param bg_colour Background color in RGB565
 */
void display_draw_battery_icon(uint16_t x, uint16_t y, uint8_t percent, uint16_t fg_colour, uint16_t bg_colour);

/**
 * @ingroup display_driver
 * @brief Draw cellular signal strength bars
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 * @param strength Signal strength (0-5 bars)
 * @param fg_colour Foreground color in RGB565
 * @param bg_colour Background color in RGB565
 */
void display_draw_signal_bars(uint16_t x, uint16_t y, uint8_t strength, uint16_t fg_colour, uint16_t bg_colour);

/**
 * @ingroup display_driver
 * @brief Draw a monochrome bitmap
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 * @param bitmap Pointer to bitmap data (1 bit per pixel)
 * @param width Bitmap width in pixels
 * @param height Bitmap height in pixels
 * @param fg_colour Foreground color in RGB565 (for set bits)
 * @param bg_colour Background color in RGB565 (for clear bits)
 */
void display_draw_mono_bitmap(uint16_t x, uint16_t y, const uint8_t *bitmap, uint16_t width, uint16_t height, uint16_t fg_colour, uint16_t bg_colour);

#endif /* DISPLAY_H */