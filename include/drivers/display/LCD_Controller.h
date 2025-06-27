/*
 * LCD_Controller.h
 *
 *  Created on: Apr 1, 2024
 *      Author: arunrawat
 */

#ifndef INC_LCD_CONTROLLER_H_
#define INC_LCD_CONTROLLER_H_

#define FMC_BANK1_REG  ((volatile uint16_t *) 0xC0000000)  // A0 = 0: Register
#define FMC_BANK1_DATA ((volatile uint16_t *) 0xC0020000)  // A0 = 1: Data

#include "main.h"

 void     LCD_IO_Init(void);
 void     LCD_IO_WriteMultipleData(uint16_t *pData, uint32_t Size);
 void     LCD_IO_WriteReg(uint8_t Reg);
 void     LCD_IO_WriteData(uint16_t RegValue);
 uint16_t LCD_IO_ReadData(void);
 void     LCD_IO_Delay(uint32_t delay);


#endif /* INC_LCD_CONTROLLER_H_ */
