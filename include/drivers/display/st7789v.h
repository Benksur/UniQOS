#ifndef ST7789V_H
#define ST7789V_H

#include "stm32_config.h"
#include "lvgl.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>



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

typedef struct {
    uint16_t width;
    uint16_t height;
    volatile uint16_t *cmd_reg;
    volatile uint16_t *data_reg;
    uint8_t madctl_val;
} st7789v_handle_t;

void st7789v_init(st7789v_handle_t *handle, uint16_t width, uint16_t height);
void st7789v_reset(st7789v_handle_t *handle);
void st7789v_write_cmd(st7789v_handle_t *handle, uint8_t cmd);
void st7789v_write_data(st7789v_handle_t *handle, uint8_t *data, size_t len);
void st7789v_write_data16(st7789v_handle_t *handle, uint16_t *data, size_t len);
void st7789v_set_pixel_format(st7789v_handle_t *handle, uint8_t format);
void st7789v_set_madctl(st7789v_handle_t *handle, uint8_t madctl_value);
void st7789v_set_address_window(st7789v_handle_t *handle, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void st7789v_fill_rect(st7789v_handle_t *handle, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void st7789v_display_on(st7789v_handle_t *handle);
void st7789v_display_off(st7789v_handle_t *handle);

#endif
