#include "nau88c22.h"
#include "stm32_config.h"
#include "stm32f4xx_hal.h"

static uint8_t codec_initialized = 0;

uint8_t nau88c22_write_reg(uint8_t reg_addr, uint16_t reg_data)
{
    uint8_t data[3];
    data[0] = reg_addr;
    data[1] = (reg_data >> 8) & 0xFF;
    data[2] = reg_data & 0xFF;

    if (HAL_I2C_Master_Transmit(&AUDIO_I2C_HANDLE, NAU88C22_I2C_ADDR << 1, data, 3, 100) != HAL_OK)
    {
        return 0;
    }

    return 1;
}

uint8_t nau88c22_read_reg(uint8_t reg_addr, uint16_t *reg_data)
{
    uint8_t data[2];

    if (HAL_I2C_Master_Transmit(&AUDIO_I2C_HANDLE, NAU88C22_I2C_ADDR << 1, &reg_addr, 1, 100) != HAL_OK)
    {
        return 0;
    }

    if (HAL_I2C_Master_Receive(&AUDIO_I2C_HANDLE, NAU88C22_I2C_ADDR << 1, data, 2, 100) != HAL_OK)
    {
        return 0;
    }

    *reg_data = (data[0] << 8) | data[1];
    return 1;
}

uint8_t nau88c22_init(void)
{
    typedef struct
    {
        uint8_t reg;
        uint16_t val;
    } reg_conf_t;

    const reg_conf_t init_seq[] = {
        {NAU_PWR1, 0x0FC},
        {NAU_PWR2, 0x1E3},
        {NAU_PWR3, 0x00C},
        {NAU_AINTF, 0x002},
        {NAU_CLOCK1, 0x073},
        {NAU_DAC_CTRL, 0x000},
        {NAU_LEFT_DAC_VOL, 0x1FF},
        {NAU_RIGHT_DAC_VOL, 0x1FF},
        {NAU_LEFT_ADC_VOL, 0x1FF},
        {NAU_RIGHT_ADC_VOL, 0x1FF}};

    uint8_t i;

    if (!nau88c22_write_reg(NAU_RESET, 0x000))
        return 0;

    HAL_Delay(50);

    for (i = 0; i < sizeof(init_seq) / sizeof(init_seq[0]); i++)
    {
        if (!nau88c22_write_reg(init_seq[i].reg, init_seq[i].val))
            return 0;
    }

    codec_initialized = 1;
    return 1;
}

uint8_t nau88c22_set_volume(uint8_t volume)
{
    if (!codec_initialized)
        return 0;

    uint16_t vol_reg;

    if (volume == 0)
    {
        vol_reg = 0x000;
    }
    else
    {
        vol_reg = (volume * 255) / 100;
        vol_reg = vol_reg & 0xFF;
        vol_reg |= 0x100;
    }

    if (!nau88c22_write_reg(NAU_LEFT_DAC_VOL, vol_reg))
        return 0;

    return nau88c22_write_reg(NAU_RIGHT_DAC_VOL, vol_reg);
}

uint8_t nau88c22_mute(uint8_t enable)
{
    if (!codec_initialized)
        return 0;

    uint16_t reg_data;

    if (!nau88c22_read_reg(NAU_DAC_CTRL, &reg_data))
        return 0;

    if (enable)
        reg_data |= 0x008;
    else
        reg_data &= ~0x008;

    return nau88c22_write_reg(NAU_DAC_CTRL, reg_data);
}

uint8_t nau88c22_set_eq(uint8_t band, uint8_t gain)
{
    if (!codec_initialized || band > 4)
        return 0;

    uint16_t gain_val = gain & 0x1F;

    uint8_t eq_reg;
    switch (band)
    {
    case 0:
        eq_reg = NAU_EQ1;
        break;
    case 1:
        eq_reg = NAU_EQ2;
        break;
    case 2:
        eq_reg = NAU_EQ3;
        break;
    case 3:
        eq_reg = NAU_EQ4;
        break;
    case 4:
        eq_reg = NAU_EQ5;
        break;
    default:
        return 0;
    }

    return nau88c22_write_reg(eq_reg, gain_val);
}