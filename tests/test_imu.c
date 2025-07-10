
#include <string.h>
#include <stdio.h>
#include "gpio.h"
#include "i2c.h"
#include "stm32_config.h"
#include "main.h"
#include "lsm6dsv.h"

int main(void)
{

  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_I2C1_Init();

  uint8_t databuff, databuff2;
  HAL_StatusTypeDef ret;
  while (1)
  {
    ret = lsm6dsv_read_reg(LSM6DSV_WHO_AM_I, &databuff); // returns 0b01110000 always
    HAL_Delay(1000);
    if (ret || databuff != 0b01110000){
      printf("FAILED SOMEWHERE\n\r");
      HAL_Delay(1);
    }

    ret = lsm6dsv_read_reg(LSM6DSV_OUT_TEMP_L, &databuff);
    ret |= lsm6dsv_read_reg(LSM6DSV_OUT_TEMP_H, &databuff2);
    if (ret){
      printf("FAILED SOMEWHERE\n\r");
      HAL_Delay(1);
    }

    printf("DONE!");
    HAL_Delay(1000);
  }

}