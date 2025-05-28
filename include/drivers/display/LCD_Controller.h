/*
 * LCD_Controller.h
 *
 *  Created on: Apr 1, 2024
 *      Author: arunrawat
 */

#ifndef INC_LCD_CONTROLLER_H_
#define INC_LCD_CONTROLLER_H_

#include "main.h"

 void     LCD_IO_Init(void);
 void     LCD_IO_WriteMultipleData(uint16_t *pData, uint32_t Size);
 void     LCD_IO_WriteReg(uint8_t Reg);
 void     LCD_IO_WriteData(uint16_t RegValue);
 uint16_t LCD_IO_ReadData(void);
 void     LCD_IO_Delay(uint32_t delay);


#endif /* INC_LCD_CONTROLLER_H_ */
