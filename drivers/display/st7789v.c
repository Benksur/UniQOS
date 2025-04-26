#include <stdint.h>
#include "st7789v.h"

void st7789v_write_cmd(const uint8_t cmd) {
    ST7789V_CMDWRITE(cmd);
}

void st7789v_write_cmd_buffer(const uint8_t *cmd, size_t len) {
    for (size_t i = 0; i < len; i++) {
        ST7789V_CMDWRITE(cmd[i]);
    }
}

void st7789v_write_data(const uint8_t data) {
    ST7789V_DATAWRITE(data);
}

void st7789v_write_data_buffer(const uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        ST7789V_DATAWRITE(data[i]);
    }
}

void st7789v_reset(void) {
    HAL_GPIO_WritePin(ST7789V_RES_GPIO_Port, ST7789V_RES_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(ST7789V_RES_GPIO_Port, ST7789V_RES_Pin, GPIO_PIN_SET);
    HAL_Delay(120);
}

static void st7789v_io_init(void) {
    st7789v_reset();
    HAL_TIM_PWM_Start(&DISPLAY_PWM_TIMER, TIM_CHANNEL_3);
    // handle brightness
}

void st7789v_init(void) {
    st7789v_io_init();
    lv_tick_set_cb(HAL_GetTick);

}

void st7789v_display_on(void) {
    st7789v_write_cmd(ST7789V_CMD_DISPON);
}

void st7789v_display_off(void) {
    st7789v_write_cmd(ST7789V_CMD_DISPOFF);
}

/* Send short command to the LCD. This function shall wait until the transaction finishes. */
static void my_lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size)
{
    LV_UNUSED(disp);
    st7789v_write_cmd_buffer(cmd, cmd_size);
    st7789v_write_data_buffer(param, param_size);
}

/* Send large array of pixel data to the LCD. If necessary, this function has to do the byte-swapping. This function can do the transfer in the background. */
static void my_lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size)
{
    LV_UNUSED(disp);
    st7789v_write_cmd_buffer(cmd, cmd_size);

    size_t pixel_count = param_size / sizeof(uint16_t);
    st7789v_write_data_buffer(param, pixel_count);
}

