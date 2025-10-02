#include "st7789v.h"
#include "display.h"
#include "theme.h"
#include "screen.h"
#include "pages/menu.h"
#include "gpio.h"
#include "spi.h"
#include "tile.h"
#include "keypad.h"
#include "option_overlay.h"
#include "status_bar.h"

#include "nau88c22.h"
#include "bloop_optimized.h"
#include "tick_sound.h"
#include "i2c.h"
#include "i2s.h"
#include "rtc.h"

#include <stdbool.h>

int16_t tick[] = {
  // sharp attack
  0, 8000, 16380, 24000, 32760, 32760, 32760, 32760, 32760, 32760,
  // sustain
  30000, 28000, 26000, 24000, 22000, 20000, 18000, 16000, 14000, 12000,
  // decay
  10000, 8000, 6000, 4000, 3000, 2500, 2000, 1500, 1000, 800,
  600, 400, 300, 200, 150, 100, 75, 50, 25, 10,
  5, 2, 1, 0, 0, 0, 0, 0, 0, 0};

int16_t bloop_base[] = {
  0, 8148, 15715, 22237, 27311, 30620, 31963, 31277, 28634, 24224,
  18349, 11401, 3832, -3903, -11477, -18425, -24268, -28649, -31276, -31921,
  -30524, -27186, -22158, -15799, -8537};

void MPU_Config(void);
void SystemClock_Config(void);

static uint16_t fill_color = 0;
void LCD_Fill(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t width, uint16_t height)
{
  display_fill_rect(Xpos, Ypos, width, height, RGBCode);
}

void draw_grid(void)
{
  for (int i = 0; i < 9 * 8; i++)
  {
    int tx = i % 8;
    int ty = i / 8;
    int px, py;
    tile_to_pixels(tx, ty, &px, &py);
    display_draw_vertical_line(px + 29, py, py + 30, 0x39c7);
    display_draw_horizontal_line(px, py + 30, px + 30, 0x39c7);
  }
}

int main(void)
{
  MPU_Config();
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_SPI4_Init();
  MX_I2C1_Init();
  MX_I2S1_Init();
  MX_RTC_Init();

  // MX_TIM2_Init();
  //   HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
  //   htim2.Instance->CCR3 = 80;

  uint8_t status;
  static const IAudioDriver_t *codec = NULL;
  codec = nau88c22_get_driver();
  codec->init();
  codec->speaker.mute(false);
  uint8_t current_volume = 50;
  codec->speaker.set_volume(100);

  display_init();
  keypad_init();

  HAL_Delay(1000);

  theme_set_dark();
  draw_status_bar();
  status_bar_update_signal(5);
  status_bar_update_battery(50);

  // Initialize volume for testing

  screen_init(&menu_page);
  mark_all_tiles_dirty();
  screen_tick();
  // draw_grid();

  while (1)
  {
    // Update keypad states and handle all inputs
    for (int i = 0; i < 24; i++)
    {
      keypad_update_states();

      // Check all buttons and handle input events
      for (int button_idx = 0; button_idx < keypad_get_button_count(); button_idx++)
      {
        if (keypad_is_button_pressed(button_idx))
        {
          HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
          input_event_t event = keypad_get_button_event(button_idx);

          // Handle special cases
          if (event == INPUT_RIGHT)
          {
            screen_pop_page();
          }
          else if (event == INPUT_VOLUME_UP)
          {
            // Volume up
            if (current_volume < 100)
            {
              current_volume += 5;
              codec->speaker.set_volume(current_volume);
              status_bar_show_volume(current_volume);
            }
          }
          else if (event == INPUT_VOLUME_DOWN)
          {
            // Volume down
            if (current_volume > 0)
            {
              current_volume -= 5;
              codec->speaker.set_volume(current_volume);
              status_bar_show_volume(current_volume);
            }
          }
          else
          {
            screen_handle_input(event);
          }

          if (event >= INPUT_KEYPAD_0 && event <= INPUT_KEYPAD_9)
          {
            HAL_I2S_Transmit(&AUDIO_I2S_HANDLE, (uint16_t *)tick, 50, HAL_MAX_DELAY);
          }
          else
          {
            for (int i = 0; i < BLOOP_REPEAT_COUNT; i++)
            {
                HAL_I2S_Transmit(&AUDIO_I2S_HANDLE, (uint16_t *)bloop_base, BLOOP_BASE_SIZE, HAL_MAX_DELAY);
            }
          }
          // Play sound for numeric keypad presses
        }
        screen_tick();
      }
    }
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

  while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
  {
  }

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
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

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }
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
    HAL_Delay(100);                        // Blink rate
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
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
