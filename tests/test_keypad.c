
#include <string.h>
#include <stdio.h>
#include "gpio.h"
#include "stm32_config.h"
#include "main.h"

void read_button(GPIO_TypeDef *port, int pin, char *name) {
    // placeholders for actual button logic
    if (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_RESET) {
      printf("got press on %s", name);
    }
}


int main(void)
{

  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();

  while (1)
  {
    read_button(PB_HASH_GPIO_Port, PB_HASH_Pin, "PB_HASH");
    read_button(PB_1_GPIO_Port, PB_1_Pin, "PB_1");
    read_button(PB_2_GPIO_Port, PB_2_Pin, "PB_2");
    read_button(PB_3_GPIO_Port, PB_3_Pin, "PB_3");
    read_button(PB_4_GPIO_Port, PB_4_Pin, "PB_4");
    read_button(PB_5_GPIO_Port, PB_5_Pin, "PB_5");
    read_button(PB_6_GPIO_Port, PB_6_Pin, "PB_6");
    read_button(PB_7_GPIO_Port, PB_7_Pin, "PB_7");
    read_button(PB_8_GPIO_Port, PB_8_Pin, "PB_8");
    read_button(PB_9_GPIO_Port, PB_9_Pin, "PB_9");
    read_button(PB_MENU_L_GPIO_Port, PB_MENU_L_Pin, "PB_MENU_L");
    read_button(PB_MENU_R_GPIO_Port, PB_MENU_R_Pin, "PB_MENU_R");
    read_button(PB_PWR_GPIO_Port, PB_PWR_Pin, "PB_PWR");
    read_button(PB_VOL_DOWN_GPIO_Port, PB_VOL_DOWN_Pin, "PB_VOL_DOWN");
    read_button(PB_VOL_UP_GPIO_Port, PB_VOL_UP_Pin, "PB_VOL_UP");
    read_button(PB_HANG_GPIO_Port, PB_HANG_Pin, "PB_HANG");
    read_button(PB_CALL_GPIO_Port, PB_CALL_Pin, "PB_CALL");
    read_button(PB_DPAD_UP_GPIO_Port, PB_DPAD_UP_Pin, "PB_DPAD_UP");
    read_button(PB_DPAD_DOWN_GPIO_Port, PB_DPAD_DOWN_Pin, "PB_DPAD_DOWN");
    read_button(PB_DPAD_RIGHT_GPIO_Port, PB_DPAD_RIGHT_Pin, "PB_DPAD_RIGHT");
    read_button(PB_DPAD_LEFT_GPIO_Port, PB_DPAD_LEFT_Pin, "PB_DPAD_LEFT");
    read_button(PB_DPAD_SELECT_GPIO_Port, PB_DPAD_SELECT_Pin, "PB_DPAD_SELECT");
    read_button(PB_STAR_GPIO_Port, PB_STAR_Pin, "PB_STAR");
    read_button(PB_0_GPIO_Port, PB_0_Pin, "PB_0");
  }

}