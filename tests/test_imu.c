
#include <string.h>
#include <stdio.h>
#include "gpio.h"
#include "stm32_config.h"
#include "main.h"

int main(void)
{

  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();

  while (1)
  {

  }

}