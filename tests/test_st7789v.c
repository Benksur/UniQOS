#include "st7789v.h"
#include "spi.h"
#include "gpio.h"


// static uint16_t fill_color = 0;
// void LCD_Fill(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t width, uint16_t height)
// {
//     ST7789V_SetAddressWindow(Xpos, Ypos, Xpos + width - 1, Ypos + height - 1);
//     ST7789V_WriteReg(ST7789V_RAMWR, NULL, 0); 
//     uint32_t total_pixels = (uint32_t)width * height;
//     for (uint32_t i = 0; i < total_pixels; i++) {
//         LCD_IO_WriteData(RGBCode);
//     }
// }

// #include <stdio.h>  // for printf, or replace with your debug print method

// // Test reading a register and checking the lowest 3 data bits
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

// #define MAX_RED     31  // 5 bits
// #define MAX_GREEN   63  // 6 bits
// #define MAX_BLUE    31  // 5 bits
// #define COLOR_STEP  1   // Smaller step for smoother transitions

// // Global or static variables to maintain state
// static uint8_t red = 0;
// static uint8_t green = 0;
// static uint8_t blue = 0;
// static int8_t color_phase = 0; // 0:R->Y, 1:Y->G, 2:G->C, 3:C->B, 4:B->M, 5:M->R

// uint16_t GetNextRainbowColor() {
//     switch (color_phase) {
//         case 0: // Red to Yellow (R=MAX, G increases, B=0)
//             green += COLOR_STEP;
//             if (green >= MAX_GREEN) {
//                 green = MAX_GREEN;
//                 color_phase = 1;
//             }
//             break;
//         case 1: // Yellow to Green (R decreases, G=MAX, B=0)
//             red -= COLOR_STEP;
//             if (red <= 0) { // Using <= 0 because it might go negative if COLOR_STEP is large
//                 red = 0;
//                 color_phase = 2;
//             }
//             break;
//         case 2: // Green to Cyan (R=0, G=MAX, B increases)
//             blue += COLOR_STEP;
//             if (blue >= MAX_BLUE) {
//                 blue = MAX_BLUE;
//                 color_phase = 3;
//             }
//             break;
//         case 3: // Cyan to Blue (R=0, G decreases, B=MAX)
//             green -= COLOR_STEP;
//             if (green <= 0) {
//                 green = 0;
//                 color_phase = 4;
//             }
//             break;
//         case 4: // Blue to Magenta (R increases, G=0, B=MAX)
//             red += COLOR_STEP;
//             if (red >= MAX_RED) {
//                 red = MAX_RED;
//                 color_phase = 5;
//             }
//             break;
//         case 5: // Magenta to Red (R=MAX, G=0, B decreases)
//             blue -= COLOR_STEP;
//             if (blue <= 0) {
//                 blue = 0;
//                 color_phase = 0; // Cycle back to start
//             }
//             break;
//     }
//     // Combine R, G, B components into a 16-bit color (RGB 5-6-5 format)
//     return ((red & 0x1F) << 11) | ((green & 0x3F) << 5) | (blue & 0x1F);
// }


int main(void)
{

  HAL_Init();
  MX_GPIO_Init();
  MX_SPI4_Init();
//   MX_FMC_Init();
//   MX_TIM2_Init();
//   HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
//   htim2.Instance->CCR3 = 80;
//   ST7789V_Init();
    ST7789_Init();
  

  HAL_Delay(1000);
//   uint16_t current_rainbow_color;
  ST7789_Test();
  while (1)
  {
    // ST7789_Fill_Color(RED);
    // ST7789_Fill_Color(0xFFFFFF);
    //   current_rainbow_color = GetNextRainbowColor();
    //   LCD_Fill(current_rainbow_color, 0, 0, 240, 320);
      
    //   HAL_Delay(8);
  }
}
