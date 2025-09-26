#pragma once
#include <stdint.h>

typedef struct {
    void (*init)(void);
    void (*fill)(uint16_t colour);
    void (*fill_rect)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t colour);
    void (*draw_pixel)(uint16_t x, uint16_t y, uint16_t colour);
    void (*draw_hline)(uint16_t x, uint16_t y, uint16_t length, uint16_t colour);
    void (*draw_vline)(uint16_t x, uint16_t y, uint16_t length, uint16_t colour);
    void (*set_orientation)(uint8_t rotation);
    uint16_t (*get_width)(void);
    uint16_t (*get_height)(void);
    void (*draw_bitmap)(uint16_t x, uint16_t y, const uint8_t *bitmap, uint16_t width, uint16_t height, uint16_t fg_colour, uint16_t bg_colour);
} IDisplayDriver_t;