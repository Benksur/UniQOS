/**
  ******************************************************************************
  * @file    st7789v.c
  * @author  Modified for ST7789V
  * @brief   This file includes the LCD driver for ST7789V LCD.
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "st7789v.h"

/** @defgroup ST7789V_Private_Variables ST7789V Private Variables
  * @{
  */ 
static uint16_t WindowsXstart = 0;
static uint16_t WindowsYstart = 0;
static uint16_t WindowsXend = ST7789V_LCD_PIXEL_WIDTH-1;
static uint16_t WindowsYend = ST7789V_LCD_PIXEL_HEIGHT-1;
/**
  * @}
  */ 

/** @defgroup ST7789V_Private_Functions ST7789V Private Functions
  * @{
  */   

/**
  * @brief  Initialize the ST7789V LCD Component.
  * @param  None
  * @retval None
  */
void ST7789V_Init(void)
{
  uint8_t parameter[16];
  
  /* Initialize ST7789V low level bus layer ----------------------------------*/
  LCD_IO_Init();
  
  /* Software Reset */
  ST7789V_WriteReg(ST7789V_SWRESET, (uint8_t*)NULL, 0);
  LCD_IO_Delay(150);
  
  /* Sleep Out */
  ST7789V_WriteReg(ST7789V_SLPOUT, (uint8_t*)NULL, 0);
  LCD_IO_Delay(120);
  
  /* Color Mode - 16bit */
  parameter[0] = 0x05;
  ST7789V_WriteReg(ST7789V_COLMOD, parameter, 1);
  LCD_IO_Delay(10);
  
  /* Memory Access Control */
  parameter[0] = 0x00;  // Normal orientation
  ST7789V_WriteReg(ST7789V_MADCTL, parameter, 1);
  
  /* Porch Control */
  parameter[0] = 0x0C;
  parameter[1] = 0x0C;
  parameter[2] = 0x00;
  parameter[3] = 0x33;
  parameter[4] = 0x33;
  ST7789V_WriteReg(ST7789V_PORCTRL, parameter, 5);
  
  /* Gate Control */
  parameter[0] = 0x35;
  ST7789V_WriteReg(ST7789V_GCTRL, parameter, 1);
  
  /* VCOM Setting */
  parameter[0] = 0x19;
  ST7789V_WriteReg(ST7789V_VCOMS, parameter, 1);
  
  /* LCM Control */
  parameter[0] = 0x2C;
  ST7789V_WriteReg(ST7789V_LCMCTRL, parameter, 1);
  
  /* VDV and VRH Command Enable */
  parameter[0] = 0x01;
  parameter[1] = 0xC3;
  ST7789V_WriteReg(ST7789V_VDVVRHEN, parameter, 2);
  
  /* VRH Set */
  parameter[0] = 0x12;
  ST7789V_WriteReg(ST7789V_VRHS, parameter, 1);
  
  /* VDV Set */
  parameter[0] = 0x20;
  ST7789V_WriteReg(ST7789V_VDVS, parameter, 1);
  
  /* Frame Rate Control */
  parameter[0] = 0x00;
  ST7789V_WriteReg(ST7789V_FRCTRL2, parameter, 1);
  
  /* Power Control 1 */
  parameter[0] = 0xA4;
  parameter[1] = 0xA1;
  ST7789V_WriteReg(ST7789V_PWCTRL1, parameter, 2);
  
  /* Positive Voltage Gamma Control */
  parameter[0] = 0xD0;
  parameter[1] = 0x04;
  parameter[2] = 0x0D;
  parameter[3] = 0x11;
  parameter[4] = 0x13;
  parameter[5] = 0x2B;
  parameter[6] = 0x3F;
  parameter[7] = 0x54;
  parameter[8] = 0x4C;
  parameter[9] = 0x18;
  parameter[10] = 0x0D;
  parameter[11] = 0x0B;
  parameter[12] = 0x1F;
  parameter[13] = 0x23;
  ST7789V_WriteReg(ST7789V_PVGAMCTRL, parameter, 14);
  
  /* Negative Voltage Gamma Control */
  parameter[0] = 0xD0;
  parameter[1] = 0x04;
  parameter[2] = 0x0C;
  parameter[3] = 0x11;
  parameter[4] = 0x13;
  parameter[5] = 0x2C;
  parameter[6] = 0x3F;
  parameter[7] = 0x44;
  parameter[8] = 0x51;
  parameter[9] = 0x2F;
  parameter[10] = 0x1F;
  parameter[11] = 0x1F;
  parameter[12] = 0x20;
  parameter[13] = 0x23;
  ST7789V_WriteReg(ST7789V_NVGAMCTRL, parameter, 14);
  
  /* Inversion On */
  ST7789V_WriteReg(ST7789V_INVON, (uint8_t*)NULL, 0);
  
  /* Normal Display Mode On */
  ST7789V_WriteReg(ST7789V_NORON, (uint8_t*)NULL, 0);
  LCD_IO_Delay(10);
  
  /* Display On */
  ST7789V_WriteReg(ST7789V_DISPON, (uint8_t*)NULL, 0);
  LCD_IO_Delay(120);
}

/**
  * @brief  Set the Display Orientation.
  * @param  orientation: ST7789V_ORIENTATION_PORTRAIT, ST7789V_ORIENTATION_LANDSCAPE
  *                      or ST7789V_ORIENTATION_LANDSCAPE_ROT180
  * @retval None
  */
void ST7789V_SetOrientation(uint32_t orientation)
{
  uint8_t parameter[1];
  
  switch(orientation)
  {
    case ST7789V_ORIENTATION_LANDSCAPE:
      parameter[0] = 0x60;
      break;
    case ST7789V_ORIENTATION_LANDSCAPE_ROT180:
      parameter[0] = 0xA0;
      break;
    case ST7789V_ORIENTATION_PORTRAIT:
    default:
      parameter[0] = 0x00;
      break;
  }
  ST7789V_WriteReg(ST7789V_MADCTL, parameter, 1);
}

/**
  * @brief  Enables the Display.
  * @param  None
  * @retval None
  */
void ST7789V_DisplayOn(void)
{
  /* Display On */
  ST7789V_WriteReg(ST7789V_DISPON, (uint8_t*)NULL, 0);
}

/**
  * @brief  Disables the Display.
  * @param  None
  * @retval None
  */
void ST7789V_DisplayOff(void)
{
  /* Display Off */
  ST7789V_WriteReg(ST7789V_DISPOFF, (uint8_t*)NULL, 0);
}

/**
  * @brief  Get LCD PIXEL WIDTH.
  * @param  None
  * @retval LCD PIXEL WIDTH
  */
uint16_t ST7789V_GetLcdPixelWidth(void)
{
  return ST7789V_LCD_PIXEL_WIDTH;
}

/**
  * @brief  Get LCD PIXEL HEIGHT.
  * @param  None
  * @retval LCD PIXEL HEIGHT
  */
uint16_t ST7789V_GetLcdPixelHeight(void)
{
  return ST7789V_LCD_PIXEL_HEIGHT;
}

/**
  * @brief  Read the LCD ID.
  * @param  None
  * @retval LCD ID
  */
uint16_t ST7789V_ReadID(void)
{
  LCD_IO_WriteReg(ST7789V_RDDID);
  LCD_IO_ReadData(); // Dummy read
  return LCD_IO_ReadData();
}

/**
  * @brief  Set Address Window for drawing
  * @param  x0: start X position
  * @param  y0: start Y position  
  * @param  x1: end X position
  * @param  y1: end Y position
  * @retval None
  */
void ST7789V_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  uint8_t parameter[4];
  
  // Add coordinate offsets for 240x320 displays
  // Many ST7789V 240x320 displays need these offsets
  uint16_t x_offset = 64;  // Common offset for 240x320 displays
  uint16_t y_offset = 0;   // Usually 0 for most displays
  
  x0 += x_offset;
  y0 += y_offset;
  x1 += x_offset;
  y1 += y_offset;
  
  /* Column Address Set */
  parameter[0] = (x0 >> 8) & 0xFF;
  parameter[1] = x0 & 0xFF;
  parameter[2] = (x1 >> 8) & 0xFF;
  parameter[3] = x1 & 0xFF;
  ST7789V_WriteReg(ST7789V_CASET, parameter, 4);
  
  /* Row Address Set */
  parameter[0] = (y0 >> 8) & 0xFF;
  parameter[1] = y0 & 0xFF;
  parameter[2] = (y1 >> 8) & 0xFF;
  parameter[3] = y1 & 0xFF;
  ST7789V_WriteReg(ST7789V_RASET, parameter, 4);
}

/**
  * @brief  Set Cursor position.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @retval None
  */
void ST7789V_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
  ST7789V_SetAddressWindow(Xpos, Ypos, ST7789V_LCD_PIXEL_WIDTH-1, ST7789V_LCD_PIXEL_HEIGHT-1);
}

/**
  * @brief  Write pixel.   
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @param  RGBCode: the RGB pixel color in RGB565 format
  * @retval None
  */
void ST7789V_WritePixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGBCode)
{
  /* Set Address Window */
  ST7789V_SetAddressWindow(Xpos, Ypos, Xpos, Ypos);
  
  /* Prepare to write to LCD RAM */
  ST7789V_WriteReg(ST7789V_RAMWR, (uint8_t*)NULL, 0);
  
  /* Write RAM data */
  LCD_IO_WriteData(RGBCode);
}

/**
  * @brief  Read pixel.
  * @param  Xpos: specifies the X position.
  * @param  Ypos: specifies the Y position.
  * @retval The RGB pixel color in RGB565 format
  */
uint16_t ST7789V_ReadPixel(uint16_t Xpos, uint16_t Ypos)
{
  /* Set Address Window */
  ST7789V_SetAddressWindow(Xpos, Ypos, Xpos, Ypos);
  
  /* Prepare to read LCD RAM */
  ST7789V_WriteReg(ST7789V_RAMRD, (uint8_t*)NULL, 0);
  
  /* Dummy read */
  LCD_IO_ReadData();
  
  /* Read pixel data */
  return LCD_IO_ReadData();
}

/**
  * @brief  Writes to the selected LCD register.
  * @param  Command: command value (or register address).
  * @param  Parameters: pointer on parameters value (if command uses parameters).
  * @param  NbParameters: number of command parameters (0 if no parameter)
  * @retval None
  */
void ST7789V_WriteReg(uint8_t Command, uint8_t *Parameters, uint8_t NbParameters)
{
  uint8_t i;

  /* Send command */
  LCD_IO_WriteReg(Command);
  
  /* Send command's parameters if any */
  for (i = 0; i < NbParameters; i++)
  {
    LCD_IO_WriteData8(Parameters[i]);
  }
}

/**
  * @brief  Reads the selected LCD Register.
  * @param  Command: command value (or register address).
  * @retval Register Value.
  */
uint8_t ST7789V_ReadReg(uint8_t Command)
{
  /* Send command */
  LCD_IO_WriteReg(Command);

  /* Read dummy data */
  LCD_IO_ReadData();
  
  /* Read register value */
  return (LCD_IO_ReadData());
}

/**
  * @brief  Sets a display window
  * @param  Xpos:   specifies the X bottom left position.
  * @param  Ypos:   specifies the Y bottom left position.
  * @param  Height: display window height.
  * @param  Width:  display window width.
  * @retval None
  */
void ST7789V_SetDisplayWindow(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  if (Xpos < ST7789V_LCD_PIXEL_WIDTH)
  {
    WindowsXstart = Xpos;
  }
  else
  {
    WindowsXstart = 0;
  }

  if (Ypos < ST7789V_LCD_PIXEL_HEIGHT)
  {
    WindowsYstart = Ypos;
  }
  else
  {
    WindowsYstart = 0;
  }

  if (Width + Xpos <= ST7789V_LCD_PIXEL_WIDTH)
  {
    WindowsXend = Width + Xpos - 1;
  }
  else
  {
    WindowsXend = ST7789V_LCD_PIXEL_WIDTH - 1;
  }

  if (Height + Ypos <= ST7789V_LCD_PIXEL_HEIGHT)
  {
    WindowsYend = Height + Ypos - 1;
  }
  else
  {
    WindowsYend = ST7789V_LCD_PIXEL_HEIGHT - 1;
  }
}

/**
  * @brief  Draw horizontal line.
  * @param  RGBCode: Specifies the RGB color in RGB565 format
  * @param  Xpos:     specifies the X position.
  * @param  Ypos:     specifies the Y position.
  * @param  Length:   specifies the Line length.  
  * @retval None
  */
void ST7789V_DrawHLine(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint16_t counter;
  
  /* Set Address Window */
  ST7789V_SetAddressWindow(Xpos, Ypos, Xpos + Length - 1, Ypos);
  
  /* Prepare to write to LCD RAM */
  ST7789V_WriteReg(ST7789V_RAMWR, (uint8_t*)NULL, 0);
  
  /* Send a complete line */
  for(counter = 0; counter < Length; counter++)
  {
    LCD_IO_WriteData(RGBCode);
  }  
}

/**
  * @brief  Draw vertical line.
  * @param  RGBCode: Specifies the RGB color    
  * @param  Xpos:     specifies the X position.
  * @param  Ypos:     specifies the Y position.
  * @param  Length:   specifies the Line length.  
  * @retval None
  */
void ST7789V_DrawVLine(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint16_t counter;

  /* Set Address Window */
  ST7789V_SetAddressWindow(Xpos, Ypos, Xpos, Ypos + Length - 1);
  
  /* Prepare to write to LCD RAM */
  ST7789V_WriteReg(ST7789V_RAMWR, (uint8_t*)NULL, 0);

  /* Fill a complete vertical line */
  for(counter = 0; counter < Length; counter++)
  {
    LCD_IO_WriteData(RGBCode);
  }
}

/**
  * @brief  Displays a bitmap picture.
  * @param  BmpAddress: Bmp picture address.
  * @param  Xpos: Bmp X position in the LCD
  * @param  Ypos: Bmp Y position in the LCD    
  * @retval None
  */
void ST7789V_DrawBitmap(uint16_t Xpos, uint16_t Ypos, uint8_t *pbmp)
{
  uint32_t index = 0, size = 0;
  uint32_t posY;
  uint32_t nb_line = 0;
  uint16_t Xsize = WindowsXend - WindowsXstart + 1;
  uint16_t Ysize = WindowsYend - WindowsYstart + 1;

  /* Read bitmap size */
  size = *(volatile uint16_t *) (pbmp + 2);
  size |= (*(volatile uint16_t *) (pbmp + 4)) << 16;
  /* Get bitmap data address offset */
  index = *(volatile uint16_t *) (pbmp + 10);
  index |= (*(volatile uint16_t *) (pbmp + 12)) << 16;
  size = (size - index)/2;
  pbmp += index;

  /* Set Address Window */
  ST7789V_SetAddressWindow(Xpos, Ypos, Xpos + Xsize - 1, Ypos + Ysize - 1);
  
  /* Prepare to write to LCD RAM */
  ST7789V_WriteReg(ST7789V_RAMWR, (uint8_t*)NULL, 0);

  for (posY = (Ypos + Ysize); posY > Ypos; posY--)  /* In BMP files the line order is inverted */
  {
    /* Write one line of the picture */
    LCD_IO_WriteMultipleData((uint16_t*)(pbmp + (nb_line * Xsize * 2)), Xsize);
    nb_line++;
  }
}

/**
  * @brief  Displays picture.
  * @param  pdata: picture address.
  * @param  Xpos: Image X position in the LCD
  * @param  Ypos: Image Y position in the LCD
  * @param  Xsize: Image X size in the LCD
  * @param  Ysize: Image Y size in the LCD
  * @retval None
  */
void ST7789V_DrawRGBImage(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint8_t *pdata)
{
  /* Set Address Window */
  ST7789V_SetAddressWindow(Xpos, Ypos, Xpos + Xsize - 1, Ypos + Ysize - 1);
  
  /* Prepare to write to LCD RAM */
  ST7789V_WriteReg(ST7789V_RAMWR, (uint8_t*)NULL, 0);
  
  /* Write image data */
  LCD_IO_WriteMultipleData((uint16_t*)pdata, Xsize * Ysize);
} 