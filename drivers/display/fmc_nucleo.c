/**
 * @file fmc_nucleo.c
 * @brief FMC bring-up for the ST7789V LCD on a NUCLEO-H723ZG dev board
 *
 * Pin map (matches the bench-tested wiring):
 *   D0  PD14   D1  PD15   D2  PD0    D3  PD1
 *   D4  PE7    D5  PE8    D6  PE9    D7  PE10
 *   D8  PA4    D9  PA5    D10 PE13   D11 PE14
 *   D12 PC0    D13 PD8    D14 PD9    D15 PD10
 *   DC  PD11 (FMC_A16)    RD PD4 (NOE)    WR PD5 (NWE)
 *   NE1 PC7               CS PD7 (GPIO, held low)   RES PD3 (GPIO)
 *
 * The HAL SRAM module is not enabled in this project's hal_conf, so the
 * bank is programmed through the FMC registers directly.
 */

#include "main.h"
#include "fmc_nucleo.h"

/* Bank 1 write timing in FMC kernel clocks (HCLK3 = 240MHz, 4.17ns).
 * ST7789V 8080 write cycle: twc >= 66ns, twrl/twrh >= 15ns. These give
 * ~33ns address setup + ~50ns WR low, ~88ns per write. */
#define LCD_FMC_ADDSET  8
#define LCD_FMC_ADDHLD  1
#define LCD_FMC_DATAST  12
#define LCD_FMC_BUSTURN 4

static void fmc_nucleo_gpio_init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    gpio.Alternate = GPIO_AF12_FMC;
    gpio.Pin = GPIO_PIN_4 | GPIO_PIN_5;
    HAL_GPIO_Init(GPIOA, &gpio);

    gpio.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_13 | GPIO_PIN_14;
    HAL_GPIO_Init(GPIOE, &gpio);

    gpio.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_9 |
               GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOD, &gpio);

    gpio.Alternate = GPIO_AF1_FMC;
    gpio.Pin = GPIO_PIN_0;
    HAL_GPIO_Init(GPIOC, &gpio);

    gpio.Alternate = GPIO_AF9_FMC;
    gpio.Pin = GPIO_PIN_7;
    HAL_GPIO_Init(GPIOC, &gpio);

    /* CS low (only device on the bus), RES high until fmc_init() pulses it. */
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET);

    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Alternate = 0;
    gpio.Pin = GPIO_PIN_3 | GPIO_PIN_7;
    HAL_GPIO_Init(GPIOD, &gpio);
}

/* The generated MPU_Config() makes 0x60000000-0xDFFFFFFF no-access. Open
 * the remapped bank 1 (64MB at 0xC0000000) as strongly-ordered memory so
 * LCD register/data writes are neither faulted nor reordered/merged. */
static void fmc_nucleo_mpu_init(void)
{
    MPU_Region_InitTypeDef region = {0};

    HAL_MPU_Disable();

    region.Enable = MPU_REGION_ENABLE;
    region.Number = MPU_REGION_NUMBER1;
    region.BaseAddress = 0xC0000000;
    region.Size = MPU_REGION_SIZE_64MB;
    region.SubRegionDisable = 0x00;
    region.TypeExtField = MPU_TEX_LEVEL0;
    region.AccessPermission = MPU_REGION_FULL_ACCESS;
    region.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    region.IsShareable = MPU_ACCESS_SHAREABLE;
    region.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    region.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    HAL_MPU_ConfigRegion(&region);

    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

void fmc_nucleo_init(void)
{
    fmc_nucleo_gpio_init();
    fmc_nucleo_mpu_init();

    /* FMC kernel clock is left at its reset source, HCLK3. */
    __HAL_RCC_FMC_CLK_ENABLE();

    /* BTR1: mode A asynchronous timing. CLKDIV/DATLAT unused (async). */
    FMC_Bank1_R->BTCR[1] = (LCD_FMC_ADDSET << FMC_BTRx_ADDSET_Pos) |
                           (LCD_FMC_ADDHLD << FMC_BTRx_ADDHLD_Pos) |
                           (LCD_FMC_DATAST << FMC_BTRx_DATAST_Pos) |
                           (LCD_FMC_BUSTURN << FMC_BTRx_BUSTURN_Pos) |
                           (0xFU << FMC_BTRx_CLKDIV_Pos) |
                           (0xFU << FMC_BTRx_DATLAT_Pos);

    /* BCR1: SRAM, 16-bit, writes enabled, NOR/PSRAM bank swapped to
     * 0xC0000000, bank and controller enabled. */
    FMC_Bank1_R->BTCR[0] = FMC_BCR1_FMCEN |
                           (1U << FMC_BCR1_BMAP_Pos) |
                           FMC_BCRx_WREN |
                           FMC_BCRx_MWID_0 |
                           FMC_BCRx_MBKEN;
}
