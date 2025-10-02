#include "st7789v.h"

// Static function prototypes
static void st7789v_init(void);
static void st7789v_set_orientation(uint32_t orientation);
static void st7789v_set_orientation_u8(uint8_t orientation);
static void st7789v_display_on(void);
static void st7789v_display_off(void);
static uint16_t st7789v_get_pixel_width(void);
static uint16_t st7789v_get_pixel_height(void);
static uint16_t st7789v_read_id(void);
static void st7789v_set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
static void st7789v_set_cursor(uint16_t Xpos, uint16_t Ypos);
static void st7789v_write_pixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGBCode);
static uint16_t st7789v_read_pixel(uint16_t Xpos, uint16_t Ypos);
static void st7789_write_reg(uint8_t Command, uint8_t *Parameters, uint8_t NbParameters);
static uint8_t st7789v_read_reg(uint8_t Command);
static void st7789v_set_display_window(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
static void st7789v_draw_hline(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length);
static void st7789v_draw_vline(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length);
static void st7789v_fill(uint16_t RGBCode);
static void st7789v_fill_rect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height, uint16_t RGBCode);

static uint16_t WindowsXstart = 0;
static uint16_t WindowsYstart = 0;
static uint16_t WindowsXend = ST7789V_LCD_PIXEL_WIDTH - 1;
static uint16_t WindowsYend = ST7789V_LCD_PIXEL_HEIGHT - 1;

static const ILCD_t *lcd = NULL;

static void st7789v_init(void)
{
  uint8_t parameter[16];
  if (!lcd) lcd = lcd_create_spi();
  lcd->init();

  /* Software Reset */
  st7789_write_reg(ST7789V_SWRESET, (uint8_t *)NULL, 0);
  lcd->delay(150);

  /* Sleep Out */
  st7789_write_reg(ST7789V_SLPOUT, (uint8_t *)NULL, 0);
  lcd->delay(120);

  /* Color Mode - 16bit */
  parameter[0] = 0x05;
  st7789_write_reg(ST7789V_COLMOD, parameter, 1);
  lcd->delay(10);

  /* Memory Access Control */
  parameter[0] = 0x00; // Normal orientation
  st7789_write_reg(ST7789V_MADCTL, parameter, 1);

  /* Porch Control */
  parameter[0] = 0x0C;
  parameter[1] = 0x0C;
  parameter[2] = 0x00;
  parameter[3] = 0x33;
  parameter[4] = 0x33;
  st7789_write_reg(ST7789V_PORCTRL, parameter, 5);

  /* Gate Control */
  parameter[0] = 0x35;
  st7789_write_reg(ST7789V_GCTRL, parameter, 1);

  /* VCOM Setting */
  parameter[0] = 0x20;
  st7789_write_reg(ST7789V_VCOMS, parameter, 1);

  /* LCM Control */
  parameter[0] = 0x2C;
  st7789_write_reg(ST7789V_LCMCTRL, parameter, 1);

  // /* VDV and VRH Command Enable */
  // parameter[0] = 0x01;
  // parameter[1] = 0xFF;
  // st7789_write_reg(ST7789V_VDVVRHEN, parameter, 2);

  // /* VRH Set */
  // parameter[0] = 0x12;
  // st7789_write_reg(ST7789V_VRHS, parameter, 1);

  // /* VDV Set */
  // parameter[0] = 0x20;
  // st7789_write_reg(ST7789V_VDVS, parameter, 1);

  /* Frame Rate Control */
  parameter[0] = 0x02;
  st7789_write_reg(ST7789V_FRCTRL2, parameter, 1);

  // /* Power Control 1 */
  // parameter[0] = 0xA4;
  // parameter[1] = 0xA1;
  // st7789_write_reg(ST7789V_PWCTRL1, parameter, 2);

  /* Positive Voltage Gamma Control */
  parameter[0] = 0xD0;
  parameter[1] = 0x04;
  parameter[2] = 0x0D;
  parameter[3] = 0x11;
  parameter[4] = 0x13;
  parameter[5] = 0x2B;
  parameter[6] = 0x3F;
  parameter[7] = 0x54;
  parameter[8] = 0x4C;
  parameter[9] = 0x18;
  parameter[10] = 0x0D;
  parameter[11] = 0x0B;
  parameter[12] = 0x1F;
  parameter[13] = 0x23;
  st7789_write_reg(ST7789V_PVGAMCTRL, parameter, 14);

  /* Negative Voltage Gamma Control */
  parameter[0] = 0xD0;
  parameter[1] = 0x04;
  parameter[2] = 0x0C;
  parameter[3] = 0x11;
  parameter[4] = 0x13;
  parameter[5] = 0x2C;
  parameter[6] = 0x3F;
  parameter[7] = 0x44;
  parameter[8] = 0x51;
  parameter[9] = 0x2F;
  parameter[10] = 0x1F;
  parameter[11] = 0x1F;
  parameter[12] = 0x20;
  parameter[13] = 0x23;
  st7789_write_reg(ST7789V_NVGAMCTRL, parameter, 14);

  /* Inversion On */
  st7789_write_reg(ST7789V_INVON, (uint8_t *)NULL, 0);

  /* Normal Display Mode On */
  st7789_write_reg(ST7789V_NORON, (uint8_t *)NULL, 0);
  lcd->delay(10);

  /* Display On */
  st7789_write_reg(ST7789V_DISPON, (uint8_t *)NULL, 0);
  lcd->delay(120);
}

static void st7789v_set_orientation(uint32_t orientation)
{
  uint8_t parameter[1];

  switch (orientation)
  {
  case ST7789V_ORIENTATION_LANDSCAPE:
    parameter[0] = 0x60;
    break;
  case ST7789V_ORIENTATION_LANDSCAPE_ROT180:
    parameter[0] = 0xA0;
    break;
  case ST7789V_ORIENTATION_PORTRAIT:
  default:
    parameter[0] = 0x00;
    break;
  }
  st7789_write_reg(ST7789V_MADCTL, parameter, 1);
}

// Wrapper for vtable: expects uint8_t, implementation uses uint32_t
static void st7789v_set_orientation_u8(uint8_t orientation) {
    st7789v_set_orientation((uint32_t)orientation);
}

static void st7789v_display_on(void)
{
  st7789_write_reg(ST7789V_DISPON, (uint8_t *)NULL, 0);
}

static void st7789v_display_off(void)
{
  st7789_write_reg(ST7789V_DISPOFF, (uint8_t *)NULL, 0);
}

static uint16_t st7789v_get_pixel_width(void)
{
  return ST7789V_LCD_PIXEL_WIDTH;
}

static uint16_t st7789v_get_pixel_height(void)
{
  return ST7789V_LCD_PIXEL_HEIGHT;
}

static uint16_t st7789v_read_id(void)
{
  lcd->write_reg(ST7789V_RDDID);
  //dummy read
  lcd->read_data();
  return lcd->read_data();
}

static void st7789v_set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  uint8_t parameter[4];

  uint16_t x_offset = 0;
  uint16_t y_offset = 0;

  x0 += x_offset;
  y0 += y_offset;
  x1 += x_offset;
  y1 += y_offset;

  /* Column Address Set */
  parameter[0] = (x0 >> 8) & 0xFF;
  parameter[1] = x0 & 0xFF;
  parameter[2] = (x1 >> 8) & 0xFF;
  parameter[3] = x1 & 0xFF;
  st7789_write_reg(ST7789V_CASET, parameter, 4);

  /* Row Address Set */
  parameter[0] = (y0 >> 8) & 0xFF;
  parameter[1] = y0 & 0xFF;
  parameter[2] = (y1 >> 8) & 0xFF;
  parameter[3] = y1 & 0xFF;
  st7789_write_reg(ST7789V_RASET, parameter, 4);
}

static void st7789v_set_cursor(uint16_t Xpos, uint16_t Ypos)
{
  st7789v_set_address_window(Xpos, Ypos, ST7789V_LCD_PIXEL_WIDTH - 1, ST7789V_LCD_PIXEL_HEIGHT - 1);
}

static void st7789v_write_pixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGBCode)
{
  st7789v_set_address_window(Xpos, Ypos, Xpos, Ypos);
  st7789_write_reg(ST7789V_RAMWR, (uint8_t *)NULL, 0);
  lcd->write_data16(RGBCode);
}

static uint16_t st7789v_read_pixel(uint16_t Xpos, uint16_t Ypos)
{
  st7789v_set_address_window(Xpos, Ypos, Xpos, Ypos);
  st7789_write_reg(ST7789V_RAMRD, (uint8_t *)NULL, 0);
  lcd->read_data();
  return lcd->read_data();
}

static void st7789_write_reg(uint8_t Command, uint8_t *Parameters, uint8_t NbParameters)
{
  uint8_t i;
  lcd->write_reg(Command);
  for (i = 0; i < NbParameters; i++)
  {
    lcd->write_data8(Parameters[i]);
  }
}

static uint8_t st7789v_read_reg(uint8_t Command)
{
  lcd->write_reg(Command);
  lcd->read_data();
  return (lcd->read_data());
}

static void st7789v_set_display_window(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height)
{
  if (Xpos < ST7789V_LCD_PIXEL_WIDTH)
  {
    WindowsXstart = Xpos;
  }
  else
  {
    WindowsXstart = 0;
  }

  if (Ypos < ST7789V_LCD_PIXEL_HEIGHT)
  {
    WindowsYstart = Ypos;
  }
  else
  {
    WindowsYstart = 0;
  }

  if (Width + Xpos <= ST7789V_LCD_PIXEL_WIDTH)
  {
    WindowsXend = Width + Xpos - 1;
  }
  else
  {
    WindowsXend = ST7789V_LCD_PIXEL_WIDTH - 1;
  }

  if (Height + Ypos <= ST7789V_LCD_PIXEL_HEIGHT)
  {
    WindowsYend = Height + Ypos - 1;
  }
  else
  {
    WindowsYend = ST7789V_LCD_PIXEL_HEIGHT - 1;
  }
}

static void st7789v_draw_hline(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint16_t counter;
  st7789v_set_address_window(Xpos, Ypos, Xpos + Length - 1, Ypos);
  st7789_write_reg(ST7789V_RAMWR, (uint8_t *)NULL, 0);
  for (counter = 0; counter < Length; counter++)
  {
    lcd->write_data16(RGBCode);
  }
}

static void st7789v_draw_vline(uint16_t RGBCode, uint16_t Xpos, uint16_t Ypos, uint16_t Length)
{
  uint16_t counter;
  st7789v_set_address_window(Xpos, Ypos, Xpos, Ypos + Length - 1);
  st7789_write_reg(ST7789V_RAMWR, (uint8_t *)NULL, 0);
  for (counter = 0; counter < Length; counter++)
  {
    lcd->write_data16(RGBCode);
  }
}

void st7789v_draw_bitmap(uint16_t Xpos, uint16_t Ypos, uint8_t *pbmp)
{
  uint32_t index = 0, size = 0;
  uint32_t posY;
  uint32_t nb_line = 0;
  uint16_t Xsize = WindowsXend - WindowsXstart + 1;
  uint16_t Ysize = WindowsYend - WindowsYstart + 1;

  /* Read bitmap size */
  size = *(volatile uint16_t *)(pbmp + 2);
  size |= (*(volatile uint16_t *)(pbmp + 4)) << 16;
  /* Get bitmap data address offset */
  index = *(volatile uint16_t *)(pbmp + 10);
  index |= (*(volatile uint16_t *)(pbmp + 12)) << 16;
  size = (size - index) / 2;
  pbmp += index;

  /* Set Address Window */
  st7789v_set_address_window(Xpos, Ypos, Xpos + Xsize - 1, Ypos + Ysize - 1);

  st7789_write_reg(ST7789V_RAMWR, (uint8_t *)NULL, 0);

  for (posY = (Ypos + Ysize); posY > Ypos; posY--) /* In BMP files the line order is inverted */
  {
    /* Write one line of the picture */
    lcd->write_data((uint16_t *)(pbmp + (nb_line * Xsize * 2)), Xsize);
    nb_line++;
  }
}

void st7789v_draw_rgb_image(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize, uint8_t *pdata)
{
  st7789v_set_address_window(Xpos, Ypos, Xpos + Xsize - 1, Ypos + Ysize - 1);
  st7789_write_reg(ST7789V_RAMWR, (uint8_t *)NULL, 0);
  lcd->write_data((uint16_t *)pdata, Xsize * Ysize);
}

static void st7789v_fill(uint16_t RGBCode)
{
  uint32_t counter;
  st7789v_set_address_window(0, 0, ST7789V_LCD_PIXEL_WIDTH - 1, ST7789V_LCD_PIXEL_HEIGHT - 1);
  st7789_write_reg(ST7789V_RAMWR, (uint8_t *)NULL, 0);
  for (counter = 0; counter < (uint32_t)ST7789V_LCD_PIXEL_WIDTH * ST7789V_LCD_PIXEL_HEIGHT; counter++)
  {
    lcd->write_data16(RGBCode);
  }
}

static void st7789v_fill_rect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height, uint16_t RGBCode)
{
  st7789v_set_address_window(Xpos, Ypos, Xpos + Width - 1, Ypos + Height - 1);
  st7789_write_reg(ST7789V_RAMWR, (uint8_t *)NULL, 0);
  uint32_t total_pixels = (uint32_t)Width * Height;
  for (uint32_t i = 0; i < total_pixels; i++) {
    lcd->write_data16(RGBCode);
  }
}

static void st7789v_draw_mono_bitmap(uint16_t Xpos, uint16_t Ypos, const uint8_t *bitmap, uint16_t width, uint16_t height, uint16_t fg_colour, uint16_t bg_colour)
{
    st7789v_set_address_window(Xpos, Ypos, Xpos + width - 1, Ypos + height - 1);
    st7789_write_reg(ST7789V_RAMWR, (uint8_t *)NULL, 0);
    for (uint32_t i = 0; i < (uint32_t)width * height; i++) {
        uint8_t pixel = bitmap[i];
        uint16_t colour = pixel ? fg_colour : bg_colour;
        lcd->write_data16(colour);
    }
}

// abstract display driver interface to allow easy swapping of display drivers
const IDisplayDriver_t* st7789v_get_driver(void)
{
    static IDisplayDriver_t driver = {
        .init = st7789v_init,
        .fill = st7789v_fill,
        .fill_rect = st7789v_fill_rect,
        .draw_pixel = st7789v_write_pixel,
        .draw_hline = st7789v_draw_hline,
        .draw_vline = st7789v_draw_vline,
        .set_orientation = st7789v_set_orientation_u8,
        .get_width = st7789v_get_pixel_width,
        .get_height = st7789v_get_pixel_height,
        .draw_bitmap = st7789v_draw_mono_bitmap // Added draw_bitmap to vtable
    };
    return &driver;
}