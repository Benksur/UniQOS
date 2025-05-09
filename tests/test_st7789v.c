
#include "st7789v.h"
#include "fmc.h"

int main(void)
{

  HAL_Init();
  SystemClock_Config();
  MX_FMC_Init();

  st7789v_init();
  lv_init();
  lv_display_t *lcd_disp = lv_st7789_create(LCD_H_RES, LCD_V_RES, LV_LCD_FLAG_NONE, my_lcd_send_cmd, my_lcd_send_color);

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
