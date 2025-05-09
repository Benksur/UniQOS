#ifndef ST7789V_H
#define ST7789V_H

#include "stm32_config.h"
#include "lvgl.h"
#include "src/drivers/display/st7789/lv_st7789.h"
#include "tim.h"
#include "fmc.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define LCD_H_RES       240
#define LCD_V_RES       320

//FMC memory bank addresses
#define ST7789V_CMD_REG_ADDR  ((uint32_t)0x60000000)
#define ST7789V_DATA_REG_ADDR ((uint32_t)0x60020000) // might need to check this address

#define ST7789V_CMDWRITE(command)   *(volatile uint16_t *) (ST7789V_CMD_REG_ADDR) = (command)
#define ST7789V_DATAWRITE(data)     *(volatile uint16_t *) (ST7789V_DATA_REG_ADDR) = (data)
#define ST7789V_CMDREAD(command)    *(volatile uint16_t *) (ST7789V_CMD_REG_ADDR)
#define ST7789V_DATAREAD(data)      *(volatile uint16_t *) (ST7789V_DATA_REG_ADDR)


#define ST7789V_CMD_NOP     0x00
#define ST7789V_CMD_SWRESET 0x01
#define ST7789V_CMD_RDDID   0x04
#define ST7789V_CMD_RDDST   0x09
#define ST7789V_CMD_SLPIN   0x10
#define ST7789V_CMD_SLPOUT  0x11
#define ST7789V_CMD_PTLON   0x12
#define ST7789V_CMD_NORON   0x13
#define ST7789V_CMD_INVOFF  0x20
#define ST7789V_CMD_INVON   0x21
#define ST7789V_CMD_DISPOFF 0x28
#define ST7789V_CMD_DISPON  0x29
#define ST7789V_CMD_CASET   0x2A
#define ST7789V_CMD_RASET   0x2B
#define ST7789V_CMD_RAMWR   0x2C
#define ST7789V_CMD_RAMRD   0x2E
#define ST7789V_CMD_PTLAR   0x30
#define ST7789V_CMD_COLMOD  0x3A
#define ST7789V_CMD_MADCTL  0x36
#define ST7789V_CMD_WRMEMC  0x3C
#define ST7789V_CMD_RDMEMC  0x3E

#define ST7789V_PIXEL_FORMAT_12BIT 0x03
#define ST7789V_PIXEL_FORMAT_16BIT 0x05
#define ST7789V_PIXEL_FORMAT_18BIT 0x06

#define ST7789V_MADCTL_MY  0x80
#define ST7789V_MADCTL_MX  0x40
#define ST7789V_MADCTL_MV  0x20
#define ST7789V_MADCTL_ML  0x10
#define ST7789V_MADCTL_RGB 0x00
#define ST7789V_MADCTL_BGR 0x08
#define ST7789V_MADCTL_MH  0x04


void st7789v_init(void);
void st7789v_reset(void);
void st7789v_write_cmd(const uint8_t cmd);
void st7789v_write_cmd_buffer(const uint8_t *cmd, size_t len);
void st7789v_write_data(const uint8_t data);
void st7789v_write_data_buffer(const uint8_t *data, size_t len);
void st7789v_display_on(void);
void st7789v_display_off(void);
void my_lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size);
void my_lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size);

#endif
