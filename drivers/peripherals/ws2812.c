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

uint8_t ws2812_set_all_pixels(uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t ret = 0;
    for (int i = 0; i<WS2812_MAX_PIXELS; i++){
        ret |= ws2812_set_pixel(i, r,g,b);
    }
    return ret;
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



//based on https://www.thevfdcollective.com/blog/stm32-and-sk6812-rgbw-led


#define PWM_LO (85)

// LED parameters
#define NUM_BPP (3) // WS2812B
#define NUM_PIXELS (12)
#define NUM_BYTES (NUM_BPP * NUM_PIXELS)

// LED color buffer
uint8_t rgb_arr[NUM_BYTES] = { 0 };

// LED write buffer
#define WR_BUF_LEN (NUM_BPP * 8 * 2)
uint8_t wr_buf[WR_BUF_LEN] = { 0 };
uint_fast8_t wr_buf_p = 0;

int currBrightness;
uint32_t timeSincePress;
int RGBToggled;

uint8_t brightness_level_conversion(int brightness)
{

    // The 10 brightness levels
    if (brightness == 1) {
        return 227;
    } else if (brightness == 2) {
        return 199;
    } else if (brightness == 3) {
        return 171;
    } else if (brightness == 4) {
        return 143;
    } else if (brightness == 5) {
        return 115;
    } else if (brightness == 6) {
        return 87;
    } else if (brightness == 7) {
        return 59;
    } else if (brightness == 8) {
        return 31;
    } else {
        return 0;
    }
}

// Set a single color (RGB) to index
void led_set_RGB(uint8_t index, uint8_t r, uint8_t g, uint8_t b, int brightness)
{
    uint8_t red, green, blue;
    int temp_red, temp_green, temp_blue;

    if (brightness < -1) {
        currBrightness = 0;
    } else if (brightness > 9) {
        currBrightness = 9;
    }
    // currBrightness = currBrightness == brightness ? currBrightness : brightness;
    uint8_t brightness_factor = brightness_level_conversion(brightness);

    // Scale the RGB values based on the brightness level
    if (brightness == 0) {
        temp_red = 0;
        temp_green = 0;
        temp_blue = 0;
    } else {
        temp_red = ((int)r) - brightness_factor < 0 ? 0 : r - brightness_factor;
        temp_green = ((int)g) - brightness_factor < 0 ? 0 : g - brightness_factor;
        temp_blue = ((int)b) - brightness_factor < 0 ? 0 : b - brightness_factor;
    }

    red = (uint8_t)temp_red;
    green = (uint8_t)temp_green;
    blue = (uint8_t)temp_blue;

    rgb_arr[3 * index] = ((uint16_t)green * 0xB0) >> 8; // green;
    rgb_arr[3 * index + 1] = red; // red
    rgb_arr[3 * index + 2] = ((uint16_t)blue * 0xF0) >> 8; // blue;
}

// Set all colors to RGB and sets brightness level
void led_set_all_RGB(uint8_t r, uint8_t g, uint8_t b, int brightness)
{
    for (uint_fast8_t i = 0; i < NUM_PIXELS; ++i)
        led_set_RGB(i, r, g, b, brightness);
}

// Shuttle the data to the LEDs!
int led_render()
{
    if (wr_buf_p != 0 || WS2812_DMA_HANDLE.State != HAL_DMA_STATE_READY) {
        // Ongoing transfer, cancel!
        for (uint8_t i = 0; i < WR_BUF_LEN; ++i)
            wr_buf[i] = 0;
        wr_buf_p = 0;
        HAL_TIM_PWM_Stop_DMA(&WS2812_TIM_HANDLE, WS2812_TIM_CHANNEL);
        return 1;
    }
    // Ooh boi the first data buffer half (and the second!)
    for (uint_fast8_t i = 0; i < 8; ++i) {
        wr_buf[i] = PWM_LO << (((rgb_arr[0] << i) & 0x80) > 0);
        wr_buf[i + 8] = PWM_LO << (((rgb_arr[1] << i) & 0x80) > 0);
        wr_buf[i + 16] = PWM_LO << (((rgb_arr[2] << i) & 0x80) > 0);
        wr_buf[i + 24] = PWM_LO << (((rgb_arr[3] << i) & 0x80) > 0);
        wr_buf[i + 32] = PWM_LO << (((rgb_arr[4] << i) & 0x80) > 0);
        wr_buf[i + 40] = PWM_LO << (((rgb_arr[5] << i) & 0x80) > 0);
    }

    HAL_StatusTypeDef stat = HAL_TIM_PWM_Start_DMA(&WS2812_TIM_HANDLE, WS2812_TIM_CHANNEL, (uint32_t*)wr_buf, WR_BUF_LEN);
    int s = (int)stat;
    wr_buf_p = 2; // Since we're ready for the next buffer
    return s;
}

void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef* htim)
{
    // DMA buffer set from LED(wr_buf_p) to LED(wr_buf_p + 1)
    if (wr_buf_p < NUM_PIXELS) {
        // We're in. Fill the even buffer
        for (uint_fast8_t i = 0; i < 8; ++i) {
            wr_buf[i] = PWM_LO << (((rgb_arr[3 * wr_buf_p] << i) & 0x80) > 0);
            wr_buf[i + 8] = PWM_LO << (((rgb_arr[3 * wr_buf_p + 1] << i) & 0x80) > 0);
            wr_buf[i + 16] = PWM_LO << (((rgb_arr[3 * wr_buf_p + 2] << i) & 0x80) > 0);
        }
        wr_buf_p++;
    } else if (wr_buf_p < NUM_PIXELS + 2) {
        // Last two transfers are resets. SK6812: 64 * 1.25 us = 80 us == good enough reset
        //                               WS2812B: 48 * 1.25 us = 60 us == good enough reset
        // First half reset zero fill
        for (uint8_t i = 0; i < WR_BUF_LEN / 2; ++i)
            wr_buf[i] = 0;
        wr_buf_p++;
    }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef* htim)
{
    // DMA buffer set from LED(wr_buf_p) to LED(wr_buf_p + 1)
    if (wr_buf_p < NUM_PIXELS) {
        // We're in. Fill the odd buffer
        for (uint_fast8_t i = 0; i < 8; ++i) {
            wr_buf[i + 24] = PWM_LO << (((rgb_arr[3 * wr_buf_p] << i) & 0x80) > 0);
            wr_buf[i + 32] = PWM_LO << (((rgb_arr[3 * wr_buf_p + 1] << i) & 0x80) > 0);
            wr_buf[i + 40] = PWM_LO << (((rgb_arr[3 * wr_buf_p + 2] << i) & 0x80) > 0);
        }
        wr_buf_p++;
    } else if (wr_buf_p < NUM_PIXELS + 2) {
        // Second half reset zero fill
        for (uint8_t i = WR_BUF_LEN / 2; i < WR_BUF_LEN; ++i)
            wr_buf[i] = 0;
        ++wr_buf_p;
    } else {
        // We're done. Lean back and until next time!
        wr_buf_p = 0;
        HAL_TIM_PWM_Stop_DMA(&WS2812_TIM_HANDLE, WS2812_TIM_CHANNEL);
    }
}