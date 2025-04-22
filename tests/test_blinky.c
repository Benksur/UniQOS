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

/**
  * @brief This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  // User can add his own implementation to report the HAL error return state
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  // User can add his own implementation to report the file name and line number,
  // ex: printf("Wrong parameters value: file %s on line %d
", file, line)
}
#endif /* USE_FULL_ASSERT */ 