#include "display.h"

// Get the display driver vtable
static const IDisplayDriver_t *driver = NULL;

static const uint8_t font5x7[96][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // space
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // "
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // #
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // $
    {0x23, 0x13, 0x08, 0x64, 0x62}, // %
    {0x36, 0x49, 0x55, 0x22, 0x50}, // &
    {0x00, 0x05, 0x03, 0x00, 0x00}, // '
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // (
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // )
    {0x08, 0x2A, 0x1C, 0x2A, 0x08}, // *
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // +
    {0x00, 0x50, 0x30, 0x00, 0x00}, // ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, // -
    {0x00, 0x60, 0x60, 0x00, 0x00}, // .
    {0x20, 0x10, 0x08, 0x04, 0x02}, // /
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 2
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, // :
    {0x00, 0x56, 0x36, 0x00, 0x00}, // ;
    {0x00, 0x08, 0x14, 0x22, 0x41}, // <
    {0x14, 0x14, 0x14, 0x14, 0x14}, // =
    {0x41, 0x22, 0x14, 0x08, 0x00}, // >
    {0x02, 0x01, 0x51, 0x09, 0x06}, // ?
    {0x32, 0x49, 0x79, 0x41, 0x3E}, // @
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
    {0x7F, 0x09, 0x09, 0x01, 0x01}, // F
    {0x3E, 0x41, 0x41, 0x51, 0x32}, // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
    {0x7F, 0x02, 0x04, 0x02, 0x7F}, // M
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
    {0x7F, 0x20, 0x18, 0x20, 0x7F}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x03, 0x04, 0x78, 0x04, 0x03}, // Y
    {0x61, 0x51, 0x49, 0x45, 0x43}, // Z
    {0x00, 0x00, 0x7F, 0x41, 0x41}, // [
    {0x02, 0x04, 0x08, 0x10, 0x20}, // backslash (ask me about backslash for a funny story)
    {0x41, 0x41, 0x7F, 0x00, 0x00}, // ]
    {0x04, 0x02, 0x01, 0x02, 0x04}, // ^
    {0x40, 0x40, 0x40, 0x40, 0x40}, // _
    {0x00, 0x03, 0x05, 0x00, 0x00}, // `
    {0x20, 0x54, 0x54, 0x54, 0x78}, // a
    {0x7F, 0x48, 0x44, 0x44, 0x38}, // b
    {0x38, 0x44, 0x44, 0x44, 0x20}, // c
    {0x38, 0x44, 0x44, 0x48, 0x7F}, // d
    {0x38, 0x54, 0x54, 0x54, 0x18}, // e
    {0x08, 0x7E, 0x09, 0x01, 0x02}, // f
    {0x08, 0x14, 0x54, 0x54, 0x3C}, // g
    {0x7F, 0x08, 0x04, 0x04, 0x78}, // h
    {0x00, 0x44, 0x7D, 0x40, 0x00}, // i
    {0x20, 0x40, 0x44, 0x3D, 0x00}, // j
    {0x00, 0x7F, 0x10, 0x28, 0x44}, // k
    {0x00, 0x41, 0x7F, 0x40, 0x00}, // l
    {0x7C, 0x04, 0x18, 0x04, 0x78}, // m
    {0x7C, 0x08, 0x04, 0x04, 0x78}, // n
    {0x38, 0x44, 0x44, 0x44, 0x38}, // o
    {0x7C, 0x14, 0x14, 0x14, 0x08}, // p
    {0x08, 0x14, 0x14, 0x18, 0x7C}, // q
    {0x7C, 0x08, 0x04, 0x04, 0x08}, // r
    {0x48, 0x54, 0x54, 0x54, 0x20}, // s
    {0x04, 0x3F, 0x44, 0x40, 0x20}, // t
    {0x3C, 0x40, 0x40, 0x20, 0x7C}, // u
    {0x1C, 0x20, 0x40, 0x20, 0x1C}, // v
    {0x3C, 0x40, 0x30, 0x40, 0x3C}, // w
    {0x44, 0x28, 0x10, 0x28, 0x44}, // x
    {0x0C, 0x50, 0x50, 0x50, 0x3C}, // y
    {0x44, 0x64, 0x54, 0x4C, 0x44}, // z
    {0x00, 0x08, 0x36, 0x41, 0x00}, // {
    {0x00, 0x00, 0x7F, 0x00, 0x00}, // |
    {0x00, 0x41, 0x36, 0x08, 0x00}, // }
    {0x08, 0x04, 0x08, 0x10, 0x08}, // ~
    {0x00, 0x00, 0x00, 0x00, 0x00}  // DEL
};

static int16_t abs16(int16_t x)
{
    return x < 0 ? -x : x;
}

static void swap16(uint16_t *a, uint16_t *b)
{
    uint16_t temp = *a;
    *a = *b;
    *b = temp;
}

void display_init(void)
{
    driver = st7789v_get_driver();
    driver->init();
}

void display_fill(uint16_t colour)
{
    driver->fill(colour);
}

void display_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t colour)
{
    driver->fill_rect(x, y, width, height, colour);
}

void display_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t colour)
{
    driver->draw_hline(colour, x, y, width);
    driver->draw_hline(colour, x, y + height - 1, width);
    driver->draw_vline(colour, x, y, height);
    driver->draw_vline(colour, x + width - 1, y, height);
}

void display_draw_vertical_line(uint16_t x, uint16_t y0, uint16_t y1, uint16_t colour)
{
    if (y0 > y1)
        swap16(&y0, &y1);
    if (x > driver->get_width() || y0 > driver->get_height() || y1 > driver->get_height())
        return;

    driver->draw_vline(colour, x, y0, y1 - y0 + 1);
}

void display_draw_horizontal_line(uint16_t x0, uint16_t y, uint16_t x1, uint16_t colour)
{
    if (x0 > x1)
        swap16(&x0, &x1);
    if (y > driver->get_height() || x0 > driver->get_width() || x1 > driver->get_width())
        return;

    driver->draw_hline(colour, x0, y, x1 - x0 + 1);
}

void display_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t colour)
{
    int16_t dx = abs16(x1 - x0);
    int16_t dy = abs16(y1 - y0);
    int16_t sx = x0 < x1 ? 1 : -1;
    int16_t sy = y0 < y1 ? 1 : -1;
    int16_t err = dx - dy;
    int16_t e2;

    while (1)
    {
        driver->draw_pixel(x0, y0, colour);

        if (x0 == x1 && y0 == y1)
            break;

        e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void display_draw_circle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t colour)
{
    int16_t x = radius;
    int16_t y = 0;
    int16_t err = 0;

    while (x >= y)
    {
        driver->draw_pixel(x0 + x, y0 + y, colour);
        driver->draw_pixel(x0 + y, y0 + x, colour);
        driver->draw_pixel(x0 - y, y0 + x, colour);
        driver->draw_pixel(x0 - x, y0 + y, colour);
        driver->draw_pixel(x0 - x, y0 - y, colour);
        driver->draw_pixel(x0 - y, y0 - x, colour);
        driver->draw_pixel(x0 + y, y0 - x, colour);
        driver->draw_pixel(x0 + x, y0 - y, colour);

        if (err <= 0)
        {
            y += 1;
            err += 2 * y + 1;
        }
        if (err > 0)
        {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

void display_fill_circle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t colour)
{
    int16_t x = radius;
    int16_t y = 0;
    int16_t err = 0;

    while (x >= y)
    {
        driver->draw_hline(colour, x0 - x, y0 + y, 2 * x + 1);
        driver->draw_hline(colour, x0 - x, y0 - y, 2 * x + 1);
        driver->draw_hline(colour, x0 - y, y0 + x, 2 * y + 1);
        driver->draw_hline(colour, x0 - y, y0 - x, 2 * y + 1);

        if (err <= 0)
        {
            y += 1;
            err += 2 * y + 1;
        }
        if (err > 0)
        {
            x -= 1;
            err -= 2 * x + 1;
        }
    }
}

void display_draw_rounded_square(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t radius, uint16_t colour)
{
    if (radius > width / 2)
        radius = width / 2;
    if (radius > height / 2)
        radius = height / 2;

    driver->draw_hline(colour, x + radius, y, width - 2 * radius);
    driver->draw_hline(colour, x + radius, y + height - 1, width - 2 * radius);
    driver->draw_vline(colour, x, y + radius, height - 2 * radius);
    driver->draw_vline(colour, x + width - 1, y + radius, height - 2 * radius);

    int16_t cx, cy;
    int16_t r = radius;
    int16_t xc = r;
    int16_t yc = 0;
    int16_t err = 0;

    while (xc >= yc)
    {
        // Top-left corner
        cx = x + radius;
        cy = y + radius;
        if (cx - xc >= x && cy - yc >= y)
            driver->draw_pixel(cx - xc, cy - yc, colour);
        if (cx - yc >= x && cy - xc >= y)
            driver->draw_pixel(cx - yc, cy - xc, colour);

        // Top-right corner
        cx = x + width - radius - 1;
        cy = y + radius;
        if (cx + xc < x + width && cy - yc >= y)
            driver->draw_pixel(cx + xc, cy - yc, colour);
        if (cx + yc < x + width && cy - xc >= y)
            driver->draw_pixel(cx + yc, cy - xc, colour);

        // Bottom-left corner
        cx = x + radius;
        cy = y + height - radius - 1;
        if (cx - xc >= x && cy + yc < y + height)
            driver->draw_pixel(cx - xc, cy + yc, colour);
        if (cx - yc >= x && cy + xc < y + height)
            driver->draw_pixel(cx - yc, cy + xc, colour);

        // Bottom-right corner
        cx = x + width - radius - 1;
        cy = y + height - radius - 1;
        if (cx + xc < x + width && cy + yc < y + height)
            driver->draw_pixel(cx + xc, cy + yc, colour);
        if (cx + yc < x + width && cy + xc < y + height)
            driver->draw_pixel(cx + yc, cy + xc, colour);

        if (err <= 0)
        {
            yc += 1;
            err += 2 * yc + 1;
        }
        if (err > 0)
        {
            xc -= 1;
            err -= 2 * xc + 1;
        }
    }
}

void display_fill_rounded_square(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t radius, uint16_t colour)
{
    if (radius > width / 2)
        radius = width / 2;
    if (radius > height / 2)
        radius = height / 2;

    display_fill_rect(x, y + radius, width, height - 2 * radius, colour);
    display_fill_rect(x + radius, y, width - 2 * radius, radius, colour);
    display_fill_rect(x + radius, y + height - radius, width - 2 * radius, radius, colour);

    int16_t cx, cy;
    int16_t r = radius;
    int16_t xc = r;
    int16_t yc = 0;
    int16_t err = 0;

    while (xc >= yc)
    {
        cx = x + radius;
        cy = y + radius;
        driver->draw_hline(colour, cx - xc, cy - yc, xc - yc + 1);
        driver->draw_hline(colour, cx - yc, cy - xc, yc + 1);

        cx = x + width - radius - 1;
        cy = y + radius;
        driver->draw_hline(colour, cx + yc, cy - xc, xc - yc + 1);
        driver->draw_hline(colour, cx, cy - yc, yc + 1);

        cx = x + radius;
        cy = y + height - radius - 1;
        driver->draw_hline(colour, cx - xc, cy + yc, xc - yc + 1);
        driver->draw_hline(colour, cx - yc, cy + xc, yc + 1);

        cx = x + width - radius - 1;
        cy = y + height - radius - 1;
        driver->draw_hline(colour, cx + yc, cy + xc, xc - yc + 1);
        driver->draw_hline(colour, cx, cy + yc, yc + 1);

        if (err <= 0)
        {
            yc += 1;
            err += 2 * yc + 1;
        }
        if (err > 0)
        {
            xc -= 1;
            err -= 2 * xc + 1;
        }
    }
}

void display_draw_triangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t colour)
{
    display_draw_line(x0, y0, x1, y1, colour);
    display_draw_line(x1, y1, x2, y2, colour);
    display_draw_line(x2, y2, x0, y0, colour);
}

void display_fill_triangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t colour)
{
    if (y0 > y1)
    {
        swap16(&y0, &y1);
        swap16(&x0, &x1);
    }
    if (y1 > y2)
    {
        swap16(&y1, &y2);
        swap16(&x1, &x2);
    }
    if (y0 > y1)
    {
        swap16(&y0, &y1);
        swap16(&x0, &x1);
    }

    int16_t dx1 = x1 - x0;
    int16_t dy1 = y1 - y0;
    int16_t dx2 = x2 - x0;
    int16_t dy2 = y2 - y0;

    for (uint16_t y = y0; y <= y2; y++)
    {
        uint16_t xa = x0 + (y - y0) * dx1 / (dy1 ? dy1 : 1);
        uint16_t xb = x0 + (y - y0) * dx2 / (dy2 ? dy2 : 1);

        if (y > y1)
        {
            int16_t dx3 = x2 - x1;
            int16_t dy3 = y2 - y1;
            xa = x1 + (y - y1) * dx3 / (dy3 ? dy3 : 1);
        }

        if (xa > xb)
            swap16(&xa, &xb);
        driver->draw_hline(colour, xa, y, xb - xa + 1);
    }
}

void display_draw_char(uint16_t x, uint16_t y, char c, uint16_t colour, uint16_t bg_colour, uint8_t size)
{
    if (c < 32 || c > 127)
        c = 32;

    const uint8_t *glyph = font5x7[c - 32];
    uint8_t i, j;

    for (i = 0; i < 5; i++)
    {
        uint8_t line = glyph[i];
        for (j = 0; j < 8; j++)
        {
            if (line & 0x01)
            {
                if (size == 1)
                {
                    driver->draw_pixel(x + i, y + j, colour);
                }
                else
                {
                    display_fill_rect(x + i * size, y + j * size, size, size, colour);
                }
            }
            else
            {
                if (size == 1)
                {
                    driver->draw_pixel(x + i, y + j, bg_colour);
                }
                else
                {
                    display_fill_rect(x + i * size, y + j * size, size, size, bg_colour);
                }
            }
            line >>= 1;
        }
    }
}

void display_draw_string(uint16_t x, uint16_t y, const char *str, uint16_t colour, uint16_t bg_colour, uint8_t size)
{
    uint16_t pos_x = x;
    while (*str)
    {
        display_draw_char(pos_x, y, *str, colour, bg_colour, size);
        pos_x += 6 * size;
        str++;
    }
}

void display_draw_bits(uint16_t x, uint16_t y, uint8_t *buff, uint16_t colour, uint16_t bg_colour, uint16_t w, uint16_t h)
{
    uint8_t i, j;

    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            uint8_t byte = buff[i * ((w + 7) / 8) + (j / 8)];
            uint8_t bit = (byte >> (7 - (j % 8))) & 0x01;

            if (bit)
            {
                driver->draw_pixel(x + j, y + i, colour);
            }
            else
            {
                driver->draw_pixel(x + j, y + i, bg_colour);
            }
        }
    }
}

void display_set_rotation(uint8_t rotation)
{
    if (!driver)
        driver = st7789v_get_driver();
    switch (rotation & 3)
    {
    case 0:
        driver->set_orientation(ST7789V_ORIENTATION_PORTRAIT);
        break;
    case 1:
        driver->set_orientation(ST7789V_ORIENTATION_LANDSCAPE);
        break;
    case 2:
        driver->set_orientation(ST7789V_ORIENTATION_LANDSCAPE_ROT180);
        break;
    case 3:
        driver->set_orientation(ST7789V_ORIENTATION_PORTRAIT);
        break;
    }
}

uint16_t display_colour565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void display_scroll_text(uint16_t x, uint16_t y, uint16_t width, const char *text, uint16_t colour, uint16_t bg_colour, uint8_t size, int16_t offset)
{
    display_fill_rect(x, y, width, 8 * size, bg_colour);

    uint16_t text_len = 0;
    const char *ptr = text;
    while (*ptr++)
        text_len++;

    uint16_t text_width = text_len * 6 * size;
    int16_t draw_x = x - offset;

    if (draw_x < x + width && draw_x + text_width > x)
    {
        display_draw_string(draw_x, y, text, colour, bg_colour, size);
    }
}

void display_draw_progress_bar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t percent, uint16_t fill_colour, uint16_t bg_colour)
{
    display_draw_rect(x, y, width, height, COLOUR_WHITE);
    display_fill_rect(x + 1, y + 1, width - 2, height - 2, bg_colour);

    if (percent > 100)
        percent = 100;
    uint16_t fill_width = ((width - 2) * percent) / 100;
    display_fill_rect(x + 1, y + 1, fill_width, height - 2, fill_colour);
}

void display_draw_battery_icon(uint16_t x, uint16_t y, uint8_t percent, uint16_t colour)
{
    display_draw_rect(x, y + 2, 20, 10, colour);
    display_fill_rect(x + 20, y + 4, 2, 6, colour);

    if (percent > 100)
        percent = 100;
    uint16_t fill_width = (18 * percent) / 100;

    if (percent > 20)
    {
        display_fill_rect(x + 1, y + 3, fill_width, 8, COLOUR_GREEN);
    }
    else
    {
        display_fill_rect(x + 1, y + 3, fill_width, 8, COLOUR_RED);
    }
}

void display_draw_signal_bars(uint16_t x, uint16_t y, uint8_t strength, uint16_t colour)
{
    uint8_t i;
    if (strength > 5)
        strength = 5;

    for (i = 0; i < 5; i++)
    {
        uint16_t bar_height = (i + 1) * 3;
        uint16_t bar_colour = (i < strength) ? colour : COLOUR_GRAY;
        display_fill_rect(x + i * 4, y + 15 - bar_height, 2, bar_height, bar_colour);
    }
}

void display_draw_mono_bitmap(uint16_t x, uint16_t y, const uint8_t *bitmap, uint16_t width, uint16_t height, uint16_t fg_colour, uint16_t bg_colour) {
    driver->draw_bitmap(x, y, bitmap, width, height, fg_colour, bg_colour);
}