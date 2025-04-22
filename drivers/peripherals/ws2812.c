#include "ws2812.h"

#define WS2812_MAX_PIXELS 17
static ws2812_pixel_t pixels[WS2812_MAX_PIXELS];

static inline __attribute__((always_inline)) void delay_cycles(uint32_t cycles) {
    asm volatile(
        "1: subs %[cycles], %[cycles], #1\n"
        "   bne 1b"
        : [cycles] "+r" (cycles)
    );
}

uint8_t ws2812_init(void) {
    HAL_GPIO_WritePin(RGB_DATA_GPIO_Port, RGB_DATA_Pin, GPIO_PIN_RESET);
    return 0;
}

uint8_t ws2812_set_pixel(uint32_t pixel_index, uint8_t r, uint8_t g, uint8_t b) {
    if (pixel_index >= WS2812_MAX_PIXELS) {
        return EINVAL;
    }
    
    pixels[pixel_index].r = r;
    pixels[pixel_index].g = g;
    pixels[pixel_index].b = b;
    
    return 0;
}

uint8_t ws2812_clear(uint32_t num_pixels) {
    if (num_pixels > WS2812_MAX_PIXELS) {
        num_pixels = WS2812_MAX_PIXELS;
    }
    
    memset(pixels, 0, num_pixels * sizeof(ws2812_pixel_t));
    return 0;
}

static void ws2812_send_bit_0(void) {
    // T0H: 0.35μs (168 cycles at 480MHz)
    // may need to tune delay
    RGB_DATA_GPIO_Port->BSRR = RGB_DATA_Pin;
    delay_cycles(128);
    
    // T0L: 0.8μs (384 cycles at 480MHz)
    RGB_DATA_GPIO_Port->BSRR = RGB_DATA_Pin << 16;
    delay_cycles(344);
}

static void ws2812_send_bit_1(void) {
    // T1H: 0.7μs (336 cycles at 480MHz)
    RGB_DATA_GPIO_Port->BSRR = RGB_DATA_Pin;
    delay_cycles(296);
    
    // T1L: 0.6μs (288 cycles at 480MHz)
    RGB_DATA_GPIO_Port->BSRR = RGB_DATA_Pin << 16;
    delay_cycles(248);
}

static void ws2812_send_byte(uint8_t byte) {
    for (int8_t bit = 7; bit >= 0; bit--) {
        if (byte & (1 << bit)) {
            ws2812_send_bit_1();
        } else {
            ws2812_send_bit_0();
        }
    }
}

uint8_t ws2812_update(uint32_t num_pixels) {
    if (num_pixels > WS2812_MAX_PIXELS) {
        return EINVAL;
    }
    
    __disable_irq();
    
    for (uint32_t i = 0; i < num_pixels; i++) {
        ws2812_send_byte(pixels[i].g);
        ws2812_send_byte(pixels[i].r);
        ws2812_send_byte(pixels[i].b);
    }
    
    __enable_irq();
    
    HAL_Delay(1);
    
    return 0;
}
