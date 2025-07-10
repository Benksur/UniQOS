
#include <string.h>
#include <stdio.h>
#include "gpio.h"
#include "i2c.h"
#include "stm32_config.h"
#include "main.h"
#include "bq27441.h"
#include "mcp73871.h"

int main(void)
{

  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_I2C1_Init();

  enum MCP73871_States status = mcp73871_status();
  while (1)
  {
    HAL_Delay(1000);
  }

}