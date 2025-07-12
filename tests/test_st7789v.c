#include "st7789v.h"
#include "tim.h"
#include "gpio.h"
#include "spi.h"

void MPU_Config(void);
void SystemClock_Config(void);

static uint16_t fill_color = 0;
void LCD_Fill(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t width, uint16_t height)
{
    ST7789V_SetAddressWindow(Xpos, Ypos, Xpos + width - 1, Ypos + height - 1);
    ST7789V_WriteReg(ST7789V_RAMWR, NULL, 0); 
    uint32_t total_pixels = (uint32_t)width * height;
    for (uint32_t i = 0; i < total_pixels; i++) {
        LCD_IO_WriteData16(RGBCode);
    }
}

#include <stdio.h>  // for printf, or replace with your debug print method

// Test reading a register and checking the lowest 3 data bits
void LCD_Test_ReadRegister(uint8_t reg)
{
    uint16_t read_value;
    LCD_IO_WriteReg(reg);
    read_value = LCD_IO_ReadData();
    uint8_t low3bits = read_value & 0x7;
    if (low3bits == 0x5)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    }
}

#define MAX_RED     31  // 5 bits
#define MAX_GREEN   63  // 6 bits
#define MAX_BLUE    31  // 5 bits
#define COLOR_STEP  1   // Smaller step for smoother transitions

// Global or static variables to maintain state
static uint8_t red = 0;
static uint8_t green = 0;
static uint8_t blue = 0;
static int8_t color_phase = 0; // 0:R->Y, 1:Y->G, 2:G->C, 3:C->B, 4:B->M, 5:M->R

uint16_t GetNextRainbowColor() {
    switch (color_phase) {
        case 0: // Red to Yellow (R=MAX, G increases, B=0)
            green += COLOR_STEP;
            if (green >= MAX_GREEN) {
                green = MAX_GREEN;
                color_phase = 1;
            }
            break;
        case 1: // Yellow to Green (R decreases, G=MAX, B=0)
            red -= COLOR_STEP;
            if (red <= 0) { // Using <= 0 because it might go negative if COLOR_STEP is large
                red = 0;
                color_phase = 2;
            }
            break;
        case 2: // Green to Cyan (R=0, G=MAX, B increases)
            blue += COLOR_STEP;
            if (blue >= MAX_BLUE) {
                blue = MAX_BLUE;
                color_phase = 3;
            }
            break;
        case 3: // Cyan to Blue (R=0, G decreases, B=MAX)
            green -= COLOR_STEP;
            if (green <= 0) {
                green = 0;
                color_phase = 4;
            }
            break;
        case 4: // Blue to Magenta (R increases, G=0, B=MAX)
            red += COLOR_STEP;
            if (red >= MAX_RED) {
                red = MAX_RED;
                color_phase = 5;
            }
            break;
        case 5: // Magenta to Red (R=MAX, G=0, B decreases)
            blue -= COLOR_STEP;
            if (blue <= 0) {
                blue = 0;
                color_phase = 0; // Cycle back to start
            }
            break;
    }
    // Combine R, G, B components into a 16-bit color (RGB 5-6-5 format)
    return ((red & 0x1F) << 11) | ((green & 0x3F) << 5) | (blue & 0x1F);
}


int main(void)
{
  MPU_Config();
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_SPI4_Init();
//   HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
//   htim2.Instance->CCR3 = 80;
  ST7789V_Init();
  

  HAL_Delay(1000);
  uint16_t current_rainbow_color;
  LCD_Fill(0xFFFF, 0, 0, 240, 320);
  while (1)
  {
      HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // Toggle LED for visual feedback
      current_rainbow_color = GetNextRainbowColor();
      LCD_Fill(current_rainbow_color, 0, 0, 240, 320);
      
    // LCD_Fill(0xFFFF, 0, 0, 240, 320);
    // HAL_Delay(1000);
    
    // LCD_Fill(0x0000, 0, 0, 240, 30);
    
    // LCD_Fill(0x0000, 0, 60, 240, 30);
    // LCD_Fill(0x0000, 0, 120, 240, 30);
    // LCD_Fill(0x0000, 0, 180, 240, 30);
    // LCD_Fill(0x0000, 0, 240, 240, 30);
    // LCD_Fill(0x0000, 0, 300, 240, 20);
    
    // HAL_Delay(1000);

    // LCD_Fill(0x0000, 30, 0, 30, 320);
    // LCD_Fill(0x0000, 90, 0, 30, 320);
    // LCD_Fill(0x0000, 150, 0, 30, 320);
    // LCD_Fill(0x0000, 210, 0, 30, 320);
    // HAL_Delay(1000);
  }
}




/**
  * @brief  The application entry point.
  * @retval int
  */

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = 64;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 30;
  RCC_OscInitStruct.PLL.PLLP = 1;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  // Add LED toggle for visual feedback
  while (1)
  {
      HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // Assuming LED is on PB0
      HAL_Delay(100); // Blink rate
  }
  /* USER CODE END Error_Handler_Debug */
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
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
