#ifndef WS2812_H
#define WS2812_H

#include <stdint.h>
#include "stm32_config.h"
#include "errno.h"

typedef struct {
    uint8_t g; 
    uint8_t r;
    uint8_t b;
} ws2812_pixel_t;

uint8_t ws2812_init(void);
uint8_t ws2812_set_pixel(uint32_t pixel_index, uint8_t r, uint8_t g, uint8_t b);
uint8_t ws2812_update(uint32_t num_pixels);
uint8_t ws2812_clear(uint32_t num_pixels);
void ws2812_deinit(void);

#endif
