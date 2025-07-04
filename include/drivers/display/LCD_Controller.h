/*
 * LCD_Controller.h
 *
 *  Created on: Apr 1, 2024
 *      Author: arunrawat
 */

#ifndef INC_LCD_CONTROLLER_H_
#define INC_LCD_CONTROLLER_H_

#include "main.h"

/* FMC */

#define FMC_BANK1_REG  ((volatile uint16_t *) 0xC0000000)  // A0 = 0: Register
#define FMC_BANK1_DATA ((volatile uint16_t *) 0xC0020000)  // A0 = 1: Data


/* SPI */


#define ST7789_SPI_PORT hspi4
extern  SPI_HandleTypeDef ST7789_SPI_PORT;
#define ST7789_CS_PORT  GPIOE
#define ST7789_CS_PIN   GPIO_PIN_11

#define ST7789_Select() HAL_GPIO_WritePin(ST7789_CS_PORT, ST7789_CS_PIN, GPIO_PIN_RESET)
#define ST7789_UnSelect() HAL_GPIO_WritePin(ST7789_CS_PORT, ST7789_CS_PIN, GPIO_PIN_SET)




 void     LCD_IO_Init(void);
 void     LCD_IO_WriteMultipleData(uint16_t *pData, uint32_t Size);
 void     LCD_IO_WriteReg(uint8_t Reg);
 void     LCD_IO_WriteData8(uint8_t data);
 void     LCD_IO_WriteData16(uint16_t data);
 uint16_t LCD_IO_ReadData(void);
 void     LCD_IO_Delay(uint32_t delay);


#endif /* INC_LCD_CONTROLLER_H_ */
