#ifndef WS2812_H
#define WS2812_H

#include "stm32h7xx_hal.h"
#include "tim.h"

#define WS2812_TIM &htim5
#define WS2812_TIM_CHANEL TIM_CHANNEL_4

// Need to match Counter Period in WS2812_TIM Period in IOC
// calculated by: (ABP2 Timer Clocks MHZ/0.8) - 1
// e.g. for 272MHZ ABP2 Timer Clock -> (272/0.8) - 1 = 339
#define BIT_TOTAL_PERIOD (htim5.Init.Period)   

// 2/3 BIT_TOTAL_PERIOD and 1/3 Respectively
#define LED_PIN_HIGH_TIME_1 ((BIT_TOTAL_PERIOD*2)/3) 
#define LED_PIN_HIGH_TIME_0 (BIT_TOTAL_PERIOD/3)

#define NUM_LEDS 17
#define BBP 3
#define LED_BUFF_SIZE (NUM_LEDS * BBP)

void ws2812_init(void);
void ws2812_update_leds(void);
uint8_t ws2812_set_brightness(uint8_t brightness);
uint8_t ws2812_set_led(uint8_t led_num, uint8_t r, uint8_t g, uint8_t b);
void ws2812_fill_led(uint8_t r, uint8_t g, uint8_t b);


#endif
