
/**
 * @file st7789v.h
 * @brief ST7789V LCD controller driver
 * @ingroup st7789v_driver
 *
 * Low-level driver for the ST7789V TFT LCD controller.
 * Provides register definitions and basic control functions for the 240x320 display.
 */

#ifndef __ST7789V_H
#define __ST7789V_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include "LCD_Controller.h"
#include "idisplay_driver.h"

/** @ingroup st7789v_driver
 *  @brief ST7789V device ID */
#define ST7789V_ID 0x85

/** @ingroup st7789v_driver
 *  @brief LCD pixel width in pixels */
#define ST7789V_LCD_PIXEL_WIDTH ((uint16_t)240)

/** @ingroup st7789v_driver
 *  @brief LCD pixel height in pixels */
#define ST7789V_LCD_PIXEL_HEIGHT ((uint16_t)320)

/** @ingroup st7789v_driver
 *  @brief Portrait orientation (240x320) */
#define ST7789V_ORIENTATION_PORTRAIT ((uint32_t)0x00)

/** @ingroup st7789v_driver
 *  @brief Landscape orientation (320x240) */
#define ST7789V_ORIENTATION_LANDSCAPE ((uint32_t)0x01)

/** @ingroup st7789v_driver
 *  @brief Landscape rotated 180° orientation */
#define ST7789V_ORIENTATION_LANDSCAPE_ROT180 ((uint32_t)0x02)

#define ST7789V_NOP 0x00
#define ST7789V_SWRESET 0x01
#define ST7789V_RDDID 0x04
#define ST7789V_RDDST 0x09
#define ST7789V_SLPIN 0x10
#define ST7789V_SLPOUT 0x11
#define ST7789V_PTLON 0x12
#define ST7789V_NORON 0x13
#define ST7789V_INVOFF 0x20
#define ST7789V_INVON 0x21
#define ST7789V_DISPOFF 0x28
#define ST7789V_DISPON 0x29
#define ST7789V_CASET 0x2A
#define ST7789V_RASET 0x2B
#define ST7789V_RAMWR 0x2C
#define ST7789V_RAMRD 0x2E
#define ST7789V_PTLAR 0x30
#define ST7789V_VSCRDEF 0x33
#define ST7789V_MADCTL 0x36
#define ST7789V_VSCRSADD 0x37
#define ST7789V_IDMOFF 0x38
#define ST7789V_IDMON 0x39
#define ST7789V_COLMOD 0x3A
#define ST7789V_RAMWRC 0x3C
#define ST7789V_RAMRDC 0x3E
#define ST7789V_TESCAN 0x44
#define ST7789V_RDTESCAN 0x45
#define ST7789V_WRDISBV 0x51
#define ST7789V_RDDISBV 0x52
#define ST7789V_WRCTRLD 0x53
#define ST7789V_RDCTRLD 0x54
#define ST7789V_WRCACE 0x55
#define ST7789V_RDCABC 0x56
#define ST7789V_WRCABCMB 0x5E
#define ST7789V_RDCABCMB 0x5F
#define ST7789V_RDABCSDR 0x68
#define ST7789V_RDID1 0xDA
#define ST7789V_RDID2 0xDB
#define ST7789V_RDID3 0xDC

// Extended commands
#define ST7789V_RAMCTRL 0xB0
#define ST7789V_RGBCTRL 0xB1
#define ST7789V_PORCTRL 0xB2
#define ST7789V_FRCTRL1 0xB3
#define ST7789V_PARCTRL 0xB5
#define ST7789V_GCTRL 0xB7
#define ST7789V_GTADJ 0xB8
#define ST7789V_DGMEN 0xBA
#define ST7789V_VCOMS 0xBB
#define ST7789V_LCMCTRL 0xC0
#define ST7789V_IDSET 0xC1
#define ST7789V_VDVVRHEN 0xC2
#define ST7789V_VRHS 0xC3
#define ST7789V_VDVS 0xC4
#define ST7789V_VMCTR1 0xC5
#define ST7789V_FRCTRL2 0xC6
#define ST7789V_CABCCTRL 0xC7
#define ST7789V_REGSEL1 0xC8
#define ST7789V_REGSEL2 0xCA
#define ST7789V_PWMFRSEL 0xCC
#define ST7789V_PWCTRL1 0xD0
#define ST7789V_VAPVANEN 0xD2
#define ST7789V_CMD2EN 0xDF
#define ST7789V_PVGAMCTRL 0xE0
#define ST7789V_NVGAMCTRL 0xE1
#define ST7789V_DGMLUTR 0xE2
#define ST7789V_DGMLUTB 0xE3
#define ST7789V_GATECTRL 0xE4
#define ST7789V_SPI2EN 0xE7
#define ST7789V_PWCTRL2 0xE8
#define ST7789V_EQCTRL 0xE9
#define ST7789V_PROMCTRL 0xEC
#define ST7789V_PROMEN 0xFA
#define ST7789V_NVMSET 0xFC
#define ST7789V_PROMACT 0xFE

#define ST7789V_TEON 0x35
#define ST7789V_TEOFF 0x34

    /**
     * @ingroup st7789v_driver
     * @brief Get the ST7789V display driver interface
     * @return Pointer to the display driver interface structure
     */
    const IDisplayDriver_t *st7789v_get_driver(void);

    /**
     * @ingroup st7789v_driver
     * @brief Draw a bitmap image to the display
     * @param Xpos X position on screen
     * @param Ypos Y position on screen
     * @param pbmp Pointer to bitmap data
     */
    void st7789v_draw_bitmap(uint16_t Xpos, uint16_t Ypos, uint8_t *pbmp);

    /**
     * @ingroup st7789v_driver
     * @brief Draw an RGB image to the display
     * @param Xpos X position on screen
     * @param Ypos Y position on screen
     * @param Xsize Width of image in pixels
     * @param Ysize Height of image in pixels
     * @param pdata Pointer to RGB image data
     */
    void st7789v_draw_rgb_image(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint8_t *pdata);

#ifdef __cplusplus
}
#endif

#endif