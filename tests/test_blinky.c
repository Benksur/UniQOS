#include "main.h"

#define LED_PORT GPIOA       
#define LED_PIN  GPIO_PIN_5  


int main(void)
{
  stm32_board_init();

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = LED_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);

  while (1)
  {
    HAL_GPIO_TogglePin(LED_PORT, LED_PIN); 
    HAL_Delay(500);
  }

  return 0; 
}

