/*
 * LCD_Controller.c
 *
 */

#include "LCD_Controller.h"

#define USE_SPI

#ifdef USE_FMC

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

void LCD_IO_WriteData8(uint8_t data)
{
    *FMC_BANK1_DATA = data && 0x00FF;
}

void LCD_IO_WriteData16(uint16_t data)
{
    *FMC_BANK1_DATA = data;
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

#endif

#ifdef USE_SPI

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
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&ST7789_SPI_PORT, (uint8_t*)&Reg, 1, HAL_MAX_DELAY);
	ST7789_UnSelect();
}

void LCD_IO_WriteData8(uint8_t data)
{
	ST7789_Select();
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);
	HAL_SPI_Transmit(&ST7789_SPI_PORT, (uint8_t*)&data, 1, HAL_MAX_DELAY);
	ST7789_UnSelect();
}

void LCD_IO_WriteData16(uint16_t data)
{
    ST7789_Select();
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);
    HAL_SPI_Transmit(&ST7789_SPI_PORT, (uint8_t*)&data, 2, HAL_MAX_DELAY);
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
        LCD_IO_WriteData16(pData[i]);
    }
}

void LCD_IO_Delay(uint32_t delay)
{
    HAL_Delay(delay);
}

#endif