/**
 * @file ws2812.c
 * @brief WS2812 RGB LED driver implementation
 *
 * This driver controls WS2812 RGB LEDs using PWM timing to generate the required
 * data signal. It supports brightness control and individual LED color setting.
 */

#include "ws2812.h"
#include "errornum.h"

uint8_t ws2812_buff[LED_BUFF_SIZE];
uint8_t ws2812_brightness;

/**
 * @brief Initialize the WS2812 LED driver
 */
void ws2812_init(void)
{
    ws2812_set_brightness(0);
    ws2812_fill_led(0xFF, 0xFF, 0xFF);
    ws2812_update_leds();
}

/**
 * @brief Update all LEDs with current buffer data
 *
 * This function sends the LED data to the WS2812 strip using PWM timing.
 * It applies brightness scaling and generates the required reset pulse.
 */
void ws2812_update_leds(void)
{
    uint8_t scaled_buff[LED_BUFF_SIZE];

    for (int i = 0; i < NUM_LEDS; i++)
    {
        scaled_buff[(i * BBP)] = (ws2812_brightness * ws2812_buff[(i * BBP)]) / 100;
        scaled_buff[(i * BBP) + 1] = (ws2812_brightness * ws2812_buff[(i * BBP) + 1]) / 100;
        scaled_buff[(i * BBP) + 2] = (ws2812_brightness * ws2812_buff[(i * BBP) + 2]) / 100;
    }

    HAL_TIM_PWM_Start(WS2812_TIM, WS2812_TIM_CHANEL);

    // align to period and use update events for one bit per pwm period
    __HAL_TIM_SET_COUNTER(WS2812_TIM, 0);
    __HAL_TIM_CLEAR_FLAG(WS2812_TIM, TIM_FLAG_UPDATE);

    for (size_t i = 0; i < LED_BUFF_SIZE; i++)
    {
        uint8_t byte = scaled_buff[i];
        for (int bit = 7; bit >= 0; bit--)
        {
            uint16_t high_time = (byte & (1 << bit)) ? LED_PIN_HIGH_TIME_1 : LED_PIN_HIGH_TIME_0;

            __HAL_TIM_SET_COMPARE(WS2812_TIM, WS2812_TIM_CHANEL, high_time);

            // wait exactly one pwm period
            while (__HAL_TIM_GET_FLAG(WS2812_TIM, TIM_FLAG_UPDATE) == RESET)
            {
            }
            __HAL_TIM_CLEAR_FLAG(WS2812_TIM, TIM_FLAG_UPDATE);
        }
    }

    // drive line low for reset (>50us) while pwm is running
    __HAL_TIM_SET_COMPARE(WS2812_TIM, WS2812_TIM_CHANEL, 0);
    for (int i = 0; i < 80; i++)
    { // ~100us at 800khz
        while (__HAL_TIM_GET_FLAG(WS2812_TIM, TIM_FLAG_UPDATE) == RESET)
        {
        }
        __HAL_TIM_CLEAR_FLAG(WS2812_TIM, TIM_FLAG_UPDATE);
    }

    HAL_TIM_PWM_Stop(WS2812_TIM, WS2812_TIM_CHANEL);
}

/**
 * @brief Set LED brightness level
 * @param brightness Brightness level (0-100%)
 * @return 0 on success, EINVAL if brightness > 100
 */
uint8_t ws2812_set_brightness(uint8_t brightness)
{
    if (brightness > 100)
    {
        return EINVAL;
    }

    ws2812_brightness = brightness;

    return 0;
}

/**
 * @brief Set color of a specific LED
 * @param led_num LED number (0 to NUM_LEDS-1)
 * @param r Red component (0-255)
 * @param g Green component (0-255)
 * @param b Blue component (0-255)
 * @return 0 on success, EINVAL if LED number is invalid
 */
uint8_t ws2812_set_led(uint8_t led_num, uint8_t r, uint8_t g, uint8_t b)
{
    if (led_num > (NUM_LEDS - 1))
    {
        return EINVAL;
    }

    ws2812_buff[led_num * BBP] = g;
    ws2812_buff[(led_num * BBP) + 1] = r;
    ws2812_buff[(led_num * BBP) + 2] = b;

    return 0;
}

/**
 * @brief Set all LEDs to the same color
 * @param r Red component (0-255)
 * @param g Green component (0-255)
 * @param b Blue component (0-255)
 */
void ws2812_fill_led(uint8_t r, uint8_t g, uint8_t b)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        ws2812_buff[(i * BBP)] = g;
        ws2812_buff[(i * BBP) + 1] = r;
        ws2812_buff[(i * BBP) + 2] = b;
    }
}