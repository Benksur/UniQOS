#include <string.h>
#include <stdio.h>
#include "gpio.h"
#include "tim.h"
#include "dma.h"
#include "stm32_config.h"
#include "main.h"
#include "ws2812.h"

int main(void)
{

    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();
    MX_DMA_Init();
    MX_TIM5_Init();
    
    led_set_all_RGB(255, 255, 255, 10);
    led_render();
    
    while (1)
    {
    HAL_Delay(100);
    led_render();

    }
}