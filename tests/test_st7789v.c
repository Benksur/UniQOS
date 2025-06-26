#include "st7789v.h"
#include "fmc.h"
#include "gpio.h"

int main(void)
{

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_FMC_Init();

  st7789v_init();
  st7789v_display_on();
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);

  // Debug: Toggle LED before lv_st7789_create
  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
  HAL_Delay(100);
  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);

  // Initialize LVGL
  lv_init();

  lv_display_t *lcd_disp = lv_st7789_create(LCD_H_RES, LCD_V_RES, LV_LCD_FLAG_NONE, my_lcd_send_cmd, my_lcd_send_color);
  
  // Debug: Toggle LED after lv_st7789_create (if we get here)
  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
  HAL_Delay(100);
  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
  
  while (1) {
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // Toggle LD1
    HAL_Delay(500);
  }

  lv_color_t * buf1 = NULL;
  lv_color_t * buf2 = NULL;

  uint32_t buf_size = LCD_H_RES * LCD_V_RES / 10 * lv_color_format_get_size(lv_display_get_color_format(lcd_disp));

  
  buf1 = lv_malloc(buf_size);
  if(buf1 == NULL) {
      LV_LOG_ERROR("display draw buffer malloc failed");
       return 1;
  }

  

  buf2 = lv_malloc(buf_size);
  if(buf2 == NULL) {
    LV_LOG_ERROR("display buffer malloc failed");
    lv_free(buf1);
    return 1;
  }
  lv_display_set_buffers(lcd_disp, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

  // ui_init(lcd_disp);

  for(;;) {
    lv_timer_handler();
    HAL_Delay(10);
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // Toggle LD1
    HAL_Delay(100);
  }

  while (1)
  {
    /* USER CODE END WHILE */
  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // Toggle LD1
  HAL_Delay(100);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}


// void LCD_Fill(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t width, uint16_t height)
// {
//     ST7789V_SetAddressWindow(Xpos, Ypos, Xpos + width - 1, Ypos + height - 1);
    
//     ST7789V_WriteReg(ST7789V_RAMWR, (uint8_t*)NULL, 0);
    
//     uint32_t total_pixels = (uint32_t)width * height;
//     for (uint32_t i = 0; i < total_pixels; i++)
//     {
//         LCD_IO_WriteData(RGBCode);
//     }
// }


// void LCD_Test_ReadRegister(uint8_t reg)
// {
//     uint16_t read_value;
//     LCD_IO_WriteReg(reg);
//     read_value = LCD_IO_ReadData();
//     uint8_t low3bits = read_value & 0x7;
//     if (low3bits == 0x5)
//     {
//         HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
//     }
//     else
//     {
//         HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
//     }
// }

// int main(void)
// {
//   HAL_Init();

//   SystemClock_Config();
//   MX_GPIO_Init();
//   MX_FMC_Init();
//   MX_TIM2_Init();

//   HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
//   htim2.Instance->CCR3 = 80;
//   ST7789V_Init();
//   ST7789V_SetDisplayWindow(0, 0, 240, 320);
  

//   HAL_Delay(1000);
  
//   while (1)
//   {
//     HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // Toggle LD1
//     HAL_Delay(1000);
    
//     // Test different colors and fill areas
//     LCD_Fill(0xF800, 0, 0, 240, 320);    // Red square
//     HAL_Delay(500);
//     LCD_Fill(0x07E0, 0, 0, 240, 320);    // Green square
//     HAL_Delay(500);
//     LCD_Fill(0x001F, 0, 0, 240, 320);    // Blue square
//     HAL_Delay(500);
//     LCD_Fill(0xFFE0, 0, 0, 240, 320);    // Yellow square
//     HAL_Delay(500);
//     LCD_Fill(0x0000, 0, 0, 240, 320);    // Clear screen (black)
//     HAL_Delay(500);
//   }
