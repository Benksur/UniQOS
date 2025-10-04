/**
 * @file LCD_Controller.h
 * @brief LCD controller interface abstraction
 * @ingroup display_controller
 *
 * Provides an abstraction layer for LCD controller communication,
 * supporting both FMC (Flexible Memory Controller) and SPI interfaces.
 * This allows the display driver to work with different hardware configurations.
 */

#ifndef INC_LCD_CONTROLLER_H_
#define INC_LCD_CONTROLLER_H_

#include "spi.h"
#include "gpio.h"

/**
 * @brief LCD I/O interface structure
 * @ingroup display_controller
 *
 * Virtual function table for LCD controller operations.
 * Provides a common interface for different communication methods.
 */
typedef struct
{
    void (*init)(void);                                 /**< Initialize the LCD interface */
    void (*write_reg)(uint8_t reg);                     /**< Write a register command */
    void (*write_data8)(uint8_t data);                  /**< Write 8-bit data */
    void (*write_data16)(uint16_t data);                /**< Write 16-bit data */
    uint16_t (*read_data)(void);                        /**< Read data from LCD */
    void (*write_data)(uint16_t *pData, uint32_t Size); /**< Write data buffer */
    void (*delay)(uint32_t delay);                      /**< Delay function */
} ILCD_t;

/**
 * @ingroup display_controller
 * @brief Create FMC-based LCD interface
 * @return Pointer to FMC LCD interface structure
 *
 * Creates an LCD interface using the Flexible Memory Controller (FMC)
 * for high-speed parallel communication.
 */
const ILCD_t *lcd_create_fmc(void);

/**
 * @ingroup display_controller
 * @brief Create SPI-based LCD interface
 * @return Pointer to SPI LCD interface structure
 *
 * Creates an LCD interface using SPI communication.
 * Suitable for lower pin count configurations.
 */
const ILCD_t *lcd_create_spi(void);

/* ==== FMC Interface specific definitions ==== */

/** @ingroup display_controller
 *  @brief FMC register bank address (A0 = 0) */
#define FMC_BANK1_REG ((volatile uint16_t *)0xC0000000)

/** @ingroup display_controller
 *  @brief FMC data bank address (A0 = 1) */
#define FMC_BANK1_DATA ((volatile uint16_t *)0xC0020000)

/* ==== SPI Interface specific definitions ==== */

/** @ingroup display_controller
 *  @brief SPI handle for ST7789 display */
#define ST7789_SPI_PORT hspi4
extern SPI_HandleTypeDef ST7789_SPI_PORT;

/** @ingroup display_controller
 *  @brief Chip select GPIO port for ST7789 */
#define ST7789_CS_PORT DISP_CS_GPIO_Port

/** @ingroup display_controller
 *  @brief Chip select GPIO pin for ST7789 */
#define ST7789_CS_PIN DISP_CS_Pin

/** @ingroup display_controller
 *  @brief Select ST7789 chip (CS low) */
#define ST7789_Select() HAL_GPIO_WritePin(ST7789_CS_PORT, ST7789_CS_PIN, GPIO_PIN_RESET)

/** @ingroup display_controller
 *  @brief Deselect ST7789 chip (CS high) */
#define ST7789_UnSelect() HAL_GPIO_WritePin(ST7789_CS_PORT, ST7789_CS_PIN, GPIO_PIN_SET)

#endif
