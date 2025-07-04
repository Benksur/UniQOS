/*
 * LCD_Controller.c
 *
 *  Created on: Apr 1, 2024
 *      Author: arunrawat
 */

#include "LCD_Controller.h"





// void LCD_IO_Init(void)
// {
//     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);
//     HAL_Delay(10);
//     HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET);
//     HAL_Delay(120);
// }

// void LCD_IO_WriteReg(uint8_t Reg)
// {
//     *FMC_BANK1_REG = (uint16_t)Reg;
// }

// void LCD_IO_WriteData(uint16_t RegValue)
// {
//     *FMC_BANK1_DATA = RegValue;
// }

// uint16_t LCD_IO_ReadData(void)
// {
//     return *FMC_BANK1_DATA;
// }

// void LCD_IO_WriteMultipleData(uint16_t *pData, uint32_t Size)
// {
//     for (uint32_t i = 0; i < Size; i++)
//     {
//         *FMC_BANK1_DATA = pData[i];
//     }
// }

void LCD_IO_Init(void)
{
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_SET);
    HAL_Delay(20);
}

void LCD_IO_WriteReg(uint8_t Reg)
{
	ST7789_Select();
    uint16_t cmd2 = Reg;
	HAL_SPI_Transmit(&ST7789_SPI_PORT, (uint8_t*)&cmd2, 1, HAL_MAX_DELAY);
	ST7789_UnSelect();
}

void LCD_IO_WriteData8(uint8_t data)
{
	ST7789_Select();
    uint16_t data2 = data|(1<<8);
	HAL_SPI_Transmit(&ST7789_SPI_PORT, (uint8_t*)&data2, 1, HAL_MAX_DELAY);
	ST7789_UnSelect();
}

void LCD_IO_WriteData16(uint16_t data)
{
    ST7789_Select();
    
    // Break 16-bit value into two 9-bit chunks with 9th bit set to 1
    uint16_t high_chunk = ((data >> 8) & 0xFF) | (1 << 8);  // High 8 bits + 9th bit set
    uint16_t low_chunk = (data & 0xFF) | (1 << 8);          // Low 8 bits + 9th bit set
    
    // Transmit high chunk first
    HAL_SPI_Transmit(&ST7789_SPI_PORT, (uint8_t*)&high_chunk, 1, HAL_MAX_DELAY);
    
    // Transmit low chunk
    HAL_SPI_Transmit(&ST7789_SPI_PORT, (uint8_t*)&low_chunk, 1, HAL_MAX_DELAY);
    
    ST7789_UnSelect();
}

uint16_t LCD_IO_ReadData(void)
{
    ST7789_Select();
    uint16_t read_data = 0;
    HAL_SPI_Receive(&ST7789_SPI_PORT, (uint8_t*)&read_data, sizeof(read_data), HAL_MAX_DELAY);
    ST7789_UnSelect();
    return read_data;
}

void LCD_IO_WriteMultipleData(uint16_t *pData, uint32_t Size)
{
    for (uint32_t i = 0; i < Size; i++)
    {
        LCD_IO_WriteData(pData[i]);
    }
}

void LCD_IO_Delay(uint32_t delay)
{
    HAL_Delay(delay);
}