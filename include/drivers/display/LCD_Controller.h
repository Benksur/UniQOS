/*
 * LCD_Controller.h
 *
 *  Created on: Apr 1, 2024
 *      Author: arunrawat
 */

#ifndef INC_LCD_CONTROLLER_H_
#define INC_LCD_CONTROLLER_H_

#include "spi.h"
#include "gpio.h"

/* ==== LCD I/O interface vtable ==== */
typedef struct {
    void (*init)(void);
    void (*write_reg)(uint8_t reg);
    void (*write_data8)(uint8_t data);
    void (*write_data16)(uint16_t data);
    uint16_t (*read_data)(void);
    void (*write_data)(uint16_t *pData, uint32_t Size);
    void (*delay)(uint32_t delay);
} ILCD_t;

const ILCD_t* lcd_create_fmc(void);
const ILCD_t* lcd_create_spi(void);

/* ==== FMC Interface specific definitions ==== */

#define FMC_BANK1_REG ((volatile uint16_t *)0xC0000000)  // A0 = 0: Register
#define FMC_BANK1_DATA ((volatile uint16_t *)0xC0020000) // A0 = 1: Data

/* ==== SPI Interface specific definitions ==== */

#define ST7789_SPI_PORT hspi4
extern SPI_HandleTypeDef ST7789_SPI_PORT;

#define ST7789_CS_PORT DISP_CS_GPIO_Port
#define ST7789_CS_PIN DISP_CS_Pin

#define ST7789_Select() HAL_GPIO_WritePin(ST7789_CS_PORT, ST7789_CS_PIN, GPIO_PIN_RESET)
#define ST7789_UnSelect() HAL_GPIO_WritePin(ST7789_CS_PORT, ST7789_CS_PIN, GPIO_PIN_SET)


#endif 
