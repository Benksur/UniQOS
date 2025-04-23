#include "ws2812.h"
#include <string.h>

#define WS2812_MAX_PIXELS 17
static ws2812_pixel_t pixels[WS2812_MAX_PIXELS];

#define WS2812_BIT_BUFFER_SIZE (24 * WS2812_MAX_PIXELS + 50)
static uint16_t dma_buffer[WS2812_BIT_BUFFER_SIZE];



#define WS2812_TIMER_PERIOD 125
#define WS2812_PWM_BIT_0 35  
#define WS2812_PWM_BIT_1 70
#define WS2812_RESET_LEN 60

#define WS2812_TIM_HANDLE htim5
#define WS2812_TIM_CHANNEL TIM_CHANNEL_4
#define WS2812_DMA_HANDLE hdma_tim5_ch4

extern TIM_HandleTypeDef WS2812_TIM_HANDLE;
extern DMA_HandleTypeDef WS2812_DMA_HANDLE;

static volatile uint8_t transfer_complete = 1;

static void WS2812_TransferComplete(DMA_HandleTypeDef *hdma)
{
    HAL_TIM_PWM_Stop_DMA(&WS2812_TIM_HANDLE, WS2812_TIM_CHANNEL);
    transfer_complete = 1;
}

uint8_t ws2812_init(void)
{
    WS2812_DMA_HANDLE.XferCpltCallback = WS2812_TransferComplete;
    __HAL_TIM_SET_AUTORELOAD(&WS2812_TIM_HANDLE, WS2812_TIMER_PERIOD - 1);
    HAL_TIM_PWM_Stop(&WS2812_TIM_HANDLE, WS2812_TIM_CHANNEL);
    
    memset(pixels, 0, sizeof(pixels));
    
    return 0;
}

uint8_t ws2812_set_pixel(uint32_t pixel_index, uint8_t r, uint8_t g, uint8_t b)
{
    if (pixel_index >= WS2812_MAX_PIXELS) {
        return EINVAL;
    }
    
    pixels[pixel_index].r = r;
    pixels[pixel_index].g = g;
    pixels[pixel_index].b = b;
    
    return 0;
}

uint8_t ws2812_clear(uint32_t num_pixels)
{
    if (num_pixels > WS2812_MAX_PIXELS) {
        num_pixels = WS2812_MAX_PIXELS;
    }
    
    memset(pixels, 0, num_pixels * sizeof(ws2812_pixel_t));
    return 0;
}

uint8_t ws2812_update(uint32_t num_pixels)
{
    if (num_pixels > WS2812_MAX_PIXELS) {
        return EINVAL;
    }
    
    if (!transfer_complete) {
        return EBUSY;
    }
    
    uint32_t buffer_index = 0;
    
    for (uint32_t i = 0; i < num_pixels; i++) {
        uint8_t color_bytes[3] = {
            pixels[i].g,
            pixels[i].r,
            pixels[i].b
        };
        
        for (uint8_t color_index = 0; color_index < 3; color_index++) {
            for (int8_t bit = 7; bit >= 0; bit--) {
                if (color_bytes[color_index] & (1 << bit)) {
                    dma_buffer[buffer_index++] = WS2812_PWM_BIT_1;
                } else {
                    dma_buffer[buffer_index++] = WS2812_PWM_BIT_0;
                }
            }
        }
    }
    
    for (uint32_t i = 0; i < WS2812_RESET_LEN; i++) {
        dma_buffer[buffer_index++] = 0;
    }
    
    transfer_complete = 0;
    if (HAL_TIM_PWM_Start_DMA(&WS2812_TIM_HANDLE, WS2812_TIM_CHANNEL, 
                             (uint32_t*)dma_buffer, buffer_index) != HAL_OK) {
        transfer_complete = 1;
        return EIO;
    }
    
    return 0;
}

void ws2812_deinit(void)
{
    HAL_TIM_PWM_Stop_DMA(&WS2812_TIM_HANDLE, WS2812_TIM_CHANNEL);
}
