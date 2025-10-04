/**
 * @file display.h
 * @brief Display driver header
 * 
 * @defgroup display_driver Display Driver
 * @brief High-level display functions for drawing graphics and text
 * @{
 * 
 * @defgroup display_basic Basic Display Functions
 * @brief Basic display initialization and control functions
 * @{
 * 
 * @defgroup display_drawing Drawing Functions
 * @brief Functions for drawing shapes, lines, and graphics
 * @{
 * 
 * @defgroup display_text Text Functions
 * @brief Functions for drawing text and characters
 * @{
 * 
 * @defgroup display_utility Utility Functions
 * @brief Utility functions for color conversion and helpers
 * @{
 */
#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include "st7789v.h"

#define COLOUR_BLACK     0x0000
#define COLOUR_WHITE     0xFFFF
#define COLOUR_RED       0xF800
#define COLOUR_GREEN     0x07E0
#define COLOUR_BLUE      0x001F
#define COLOUR_YELLOW    0xFFE0
#define COLOUR_CYAN      0x07FF
#define COLOUR_MAGENTA   0xF81F
#define COLOUR_GRAY      0x7BEF
#define COLOUR_ORANGE    0xFC20

typedef struct {
    uint16_t x;
    uint16_t y;
} point_t;

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
} rect_t;

void display_init(void);
void display_fill(uint16_t colour);
void display_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t colour);
void display_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t colour);
void display_draw_vertical_line(uint16_t x, uint16_t y0, uint16_t y1, uint16_t colour);
void display_draw_horizontal_line(uint16_t x0, uint16_t y, uint16_t x1, uint16_t colour);
void display_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t colour);
void display_draw_circle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t colour);
void display_fill_circle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t colour);
void display_draw_rounded_square(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t radius, uint16_t colour);
void display_fill_rounded_square(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t radius, uint16_t colour);
void display_draw_triangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t colour);
void display_fill_triangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t colour);
void display_draw_char(uint16_t x, uint16_t y, char c, uint16_t colour, uint16_t bg_colour, uint8_t size);
void display_draw_string(uint16_t x, uint16_t y, const char *str, uint16_t colour, uint16_t bg_colour, uint8_t size);
void display_draw_bits(uint16_t x, uint16_t y, uint8_t *buff, uint16_t colour, uint16_t bg_colour, uint16_t w, uint16_t h);
void display_set_rotation(uint8_t rotation);
uint16_t display_colour565(uint8_t r, uint8_t g, uint8_t b);
void display_scroll_text(uint16_t x, uint16_t y, uint16_t width, const char *text, uint16_t colour, uint16_t bg_colour, uint8_t size, int16_t offset);
void display_draw_progress_bar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t percent, uint16_t fill_colour, uint16_t bg_colour);
void display_draw_battery_icon(uint16_t x, uint16_t y, uint8_t percent, uint16_t fg_colour, uint16_t bg_colour);
void display_draw_signal_bars(uint16_t x, uint16_t y, uint8_t strength, uint16_t fg_colour, uint16_t bg_colour);
void display_draw_mono_bitmap(uint16_t x, uint16_t y, const uint8_t *bitmap, uint16_t width, uint16_t height, uint16_t fg_colour, uint16_t bg_colour);

#endif