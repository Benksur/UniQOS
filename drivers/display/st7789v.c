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
void my_lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size)
{
    LV_UNUSED(disp);
    st7789v_write_cmd_buffer(cmd, cmd_size);
    st7789v_write_data_buffer(param, param_size);
}

/* Send large array of pixel data to the LCD. If necessary, this function has to do the byte-swapping. This function can do the transfer in the background. */
void my_lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size)
{
    LV_UNUSED(disp);
    st7789v_write_cmd_buffer(cmd, cmd_size);

    size_t pixel_count = param_size / sizeof(uint16_t);
    st7789v_write_data_buffer(param, pixel_count);
    lv_display_flush_ready(disp);
}

/*
example display task
void task_display(void const *argument) {
    st7789v_io_init();
    lv_init();
    lv_display_t *lcd_disp = lv_st7789_create(LCD_H_RES, LCD_V_RES, LV_LCD_FLAG_NONE, lcd_send_cmd, lcd_send_color);

    lv_color_t * buf1 = NULL;
    lv_color_t * buf2 = NULL;

    uint32_t buf_size = LCD_H_RES * LCD_V_RES / 10 * lv_color_format_get_size(lv_display_get_color_format(lcd_disp));

    buf1 = lv_malloc(buf_size);
    if(buf1 == NULL) {
        LV_LOG_ERROR("display draw buffer malloc failed");
        return;
    }

    buf2 = lv_malloc(buf_size);
    if(buf2 == NULL) {
        LV_LOG_ERROR("display buffer malloc failed");
        lv_free(buf1);
        return;
    }
    lv_display_set_buffers(lcd_disp, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

    ui_init(lcd_disp);

    for(;;) {
        lv_timer_handler();
        osDelay(10);
    }



*/

