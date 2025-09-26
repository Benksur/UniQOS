#include <string.h>
#include <stdio.h>
#include "gpio.h"
#include "stm32_config.h"
#include "main.h"
#include "tim.h"
#include "drv2603.h"

int main(void)
{

  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_TIM3_Init();

  drv2603_init();
  drv2603_set_strength_lra(100);
  while (1)
  {
    HAL_Delay(1000);
    drv2603_enable(1);
    
    HAL_Delay(1000);
    drv2603_enable(0);
  }

}