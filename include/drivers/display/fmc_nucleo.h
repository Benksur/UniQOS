/**
 * @file fmc_nucleo.h
 * @brief FMC bring-up for the ST7789V LCD on a NUCLEO-H723ZG dev board
 * @ingroup display_controller
 *
 * Only built for BOARD=nucleo. The phone PCB's CubeMX config (Core/) has the
 * display on SPI and a different pin map, so on the Nucleo the FMC is set
 * up here instead, using the pin map of the bench-tested LCD wiring.
 */

#ifndef FMC_NUCLEO_H
#define FMC_NUCLEO_H

/**
 * @ingroup display_controller
 * @brief Configure FMC bank 1 (SRAM, 16-bit) for the LCD
 *
 * Muxes the LCD bus pins to the FMC, holds the LCD chip select (PD7) low,
 * sets PD3 (RESX) as an output, swaps the NOR/PSRAM bank to 0xC0000000
 * (BMAP = 01) to match FMC_BANK1_REG/FMC_BANK1_DATA in LCD_Controller.h,
 * and opens that region in the MPU. Must run after MX_GPIO_Init() and the
 * other peripheral inits, since several of them claim the same pins.
 */
void fmc_nucleo_init(void);

#endif
