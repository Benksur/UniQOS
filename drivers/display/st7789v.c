#include "st7789v.h"


#define ST7789V_CMD_REG_ADDR  ((volatile uint16_t *)0x60000000)
#define ST7789V_DATA_REG_ADDR ((volatile uint16_t *)0x60020000)

void st7789v_write_cmd(st7789v_handle_t *handle, uint8_t cmd) {
    *handle->cmd_reg = cmd;
}

void st7789v_write_data(st7789v_handle_t *handle, uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        *handle->data_reg = data[i];
    }
}

void st7789v_write_data16(st7789v_handle_t *handle, uint16_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        *handle->data_reg = data[i];
    }
}

void st7789v_reset(st7789v_handle_t *handle) {
    HAL_GPIO_WritePin(ST7789V_RES_GPIO_Port, ST7789V_RES_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(ST7789V_RES_GPIO_Port, ST7789V_RES_Pin, GPIO_PIN_SET);
    HAL_Delay(120);
}

void st7789v_init(st7789v_handle_t *handle, uint16_t width, uint16_t height) {
    handle->width = width;
    handle->height = height;
    handle->cmd_reg = ST7789V_CMD_REG_ADDR;
    handle->data_reg = ST7789V_DATA_REG_ADDR;
    handle->madctl_val = ST7789V_MADCTL_RGB;

    st7789v_reset(handle);

    st7789v_write_cmd(handle, ST7789V_CMD_SWRESET);
    HAL_Delay(150);

    st7789v_write_cmd(handle, ST7789V_CMD_SLPOUT);
    HAL_Delay(500);

    st7789v_set_pixel_format(handle, ST7789V_PIXEL_FORMAT_16BIT);
    HAL_Delay(10);

    st7789v_set_madctl(handle, handle->madctl_val);

    st7789v_display_on(handle);
    HAL_Delay(100);

    st7789v_fill_rect(handle, 0, 0, handle->width, handle->height, 0x0000);
}

void st7789v_set_pixel_format(st7789v_handle_t *handle, uint8_t format) {
    st7789v_write_cmd(handle, ST7789V_CMD_COLMOD);
    st7789v_write_data(handle, &format, 1);
}

void st7789v_set_madctl(st7789v_handle_t *handle, uint8_t madctl_value) {
    handle->madctl_val = madctl_value;
    st7789v_write_cmd(handle, ST7789V_CMD_MADCTL);
    st7789v_write_data(handle, &handle->madctl_val, 1);
}

void st7789v_set_address_window(st7789v_handle_t *handle, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    uint8_t data[4];

    st7789v_write_cmd(handle, ST7789V_CMD_CASET);
    data[0] = (x0 >> 8) & 0xFF;
    data[1] = x0 & 0xFF;
    data[2] = (x1 >> 8) & 0xFF;
    data[3] = x1 & 0xFF;
    st7789v_write_data(handle, data, 4);

    st7789v_write_cmd(handle, ST7789V_CMD_RASET);
    data[0] = (y0 >> 8) & 0xFF;
    data[1] = y0 & 0xFF;
    data[2] = (y1 >> 8) & 0xFF;
    data[3] = y1 & 0xFF;
    st7789v_write_data(handle, data, 4);

    st7789v_write_cmd(handle, ST7789V_CMD_RAMWR);
}

void st7789v_fill_rect(st7789v_handle_t *handle, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    if ((x >= handle->width) || (y >= handle->height)) return;
    if ((x + w - 1) >= handle->width) w = handle->width - x;
    if ((y + h - 1) >= handle->height) h = handle->height - y;

    st7789v_set_address_window(handle, x, y, x + w - 1, y + h - 1);

    for (uint32_t i = 0; i < (w * h); i++) {
        *handle->data_reg = color;
    }
}

void st7789v_display_on(st7789v_handle_t *handle) {
    st7789v_write_cmd(handle, ST7789V_CMD_DISPON);
}

void st7789v_display_off(st7789v_handle_t *handle) {
    st7789v_write_cmd(handle, ST7789V_CMD_DISPOFF);
}
