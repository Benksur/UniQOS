/*
 * LCD_Controller.c
 *
 */

#include "LCD_Controller.h"

static void fmc_init(void)
{
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET);
    HAL_Delay(120);
}

static void fmc_write_reg(uint8_t Reg)
{
    *FMC_BANK1_REG = (uint16_t)Reg;
}

static void fmc_write_data8(uint8_t data)
{
    *FMC_BANK1_DATA = data && 0x00FF;
}

static void fmc_write_data16(uint16_t data)
{
    *FMC_BANK1_DATA = data;
}

static uint16_t fmc_read_data(void)
{
    return *FMC_BANK1_DATA;
}

static void fmc_write_data(uint16_t *pData, uint32_t Size)
{
    for (uint32_t i = 0; i < Size; i++)
    {
        *FMC_BANK1_DATA = pData[i];
    }
}

static void fmc_delay(uint32_t delay)
{
    HAL_Delay(delay);
}

const ILCD_t *lcd_create_fmc(void)
{
    static const ILCD_t fmc_lcd_io = {
        .init = fmc_init,
        .write_reg = fmc_write_reg,
        .write_data8 = fmc_write_data8,
        .write_data16 = fmc_write_data16,
        .read_data = fmc_read_data,
        .write_data = fmc_write_data,
        .delay = fmc_delay};
    return &fmc_lcd_io;
}

static void spi_init(void)
{
    HAL_GPIO_WritePin(DISP_RES_GPIO_Port, DISP_RES_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(DISP_RES_GPIO_Port, DISP_RES_Pin, GPIO_PIN_SET);
    HAL_Delay(20);
}

static void spi_write_reg(uint8_t Reg)
{
    ST7789_Select();
    HAL_GPIO_WritePin(DISP_DC_GPIO_Port, DISP_DC_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&ST7789_SPI_PORT, (uint8_t *)&Reg, 1, HAL_MAX_DELAY);
    ST7789_UnSelect();
}

static void spi_write_data8(uint8_t data)
{
    ST7789_Select();
    HAL_GPIO_WritePin(DISP_DC_GPIO_Port, DISP_DC_Pin, GPIO_PIN_SET);
    HAL_SPI_Transmit(&ST7789_SPI_PORT, (uint8_t *)&data, 1, HAL_MAX_DELAY);
    ST7789_UnSelect();
}

static void spi_write_data16(uint16_t data)
{
    ST7789_Select();
    HAL_GPIO_WritePin(DISP_DC_GPIO_Port, DISP_DC_Pin, GPIO_PIN_SET);
    HAL_SPI_Transmit(&ST7789_SPI_PORT, (uint8_t *)&data, 2, HAL_MAX_DELAY);
    ST7789_UnSelect();
}

static uint16_t spi_read_data(void)
{
    ST7789_Select();
    uint16_t read_data = 0;
    HAL_SPI_Receive(&ST7789_SPI_PORT, (uint8_t *)&read_data, sizeof(read_data), HAL_MAX_DELAY);
    ST7789_UnSelect();
    return read_data;
}

static void spi_write_data(uint16_t *pData, uint32_t Size)
{
    for (uint32_t i = 0; i < Size; i++)
    {
        spi_write_data16(pData[i]);
    }
}

static void spi_delay(uint32_t delay)
{
    HAL_Delay(delay);
}

const ILCD_t *lcd_create_spi(void)
{
    static const ILCD_t spi_lcd_io = {
        .init = spi_init,
        .write_reg = spi_write_reg,
        .write_data8 = spi_write_data8,
        .write_data16 = spi_write_data16,
        .read_data = spi_read_data,
        .write_data = spi_write_data,
        .delay = spi_delay};
    return &spi_lcd_io;
}
