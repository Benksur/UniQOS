/*
 * LCD_Controller.c
 *
 *  Created on: Apr 1, 2024
 *      Author: arunrawat
 */

#include "LCD_Controller.h"

#define FMC_BANK1_REG  ((volatile uint16_t *) 0xC0000000)  // A0 = 0: Register
#define FMC_BANK1_DATA ((volatile uint16_t *) 0xC0020000)  // A0 = 1: Data

void LCD_IO_Init(void)
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET);
    HAL_Delay(120);
}

void LCD_IO_WriteReg(uint8_t Reg)
{
    *FMC_BANK1_REG = (uint16_t)Reg;
}

void LCD_IO_WriteData(uint16_t RegValue)
{
    *FMC_BANK1_DATA = RegValue;
}

uint16_t LCD_IO_ReadData(void)
{
    return *FMC_BANK1_DATA;
}

void LCD_IO_WriteMultipleData(uint16_t *pData, uint32_t Size)
{
    for (uint32_t i = 0; i < Size; i++)
    {
        *FMC_BANK1_DATA = pData[i];
    }
}

void LCD_IO_Delay(uint32_t delay)
{
    HAL_Delay(delay);
}