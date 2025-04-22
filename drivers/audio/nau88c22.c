#include "nau88c22.h"

static uint8_t codec_initialized = 0;

typedef struct
{
    uint8_t output_muted;
    uint8_t mic_muted;
    uint8_t hp_mic_muted;
} nau88c22_mute_state_t;

static nau88c22_mute_state_t saved_mute_state;
static uint8_t mute_state_saved = 0;

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

    const reg_conf_t startup_seq[] = {
        {NAU_PWR1, 0x004},
        {NAU_PWR1, 0x00D},
    };

    const reg_conf_t init_seq[] = {
        {NAU_AINTF, 0x011}, // Mono operation mode I want to test here....
        {NAU_CLOCK2, 0x001},
        {NAU_JACK_DETECT1, 0x040},
        {NAU_JACK_DETECT2, 0x021}, // toggle loudspeaker to headphones on jack detect
        // R = 9.408163070, f2 = 90.318MHz, f1 = 19.2/2 = 9.6MHz
        {NAU_PPL_N, 0x019},  // N of 9
        {NAU_PPL_K1, 0x1A},  // 0.408163070
        {NAU_PPL_K2, 0x039}, // 0.000000000
        {NAU_PPL_K3, 0x0B0},
        {NAU_INPUT_CONTROL, 0x0B3}, // 0.65x microphone bias
        // May need to change - Investigate with HP det
        {NAU_OUTPUT_CONTROL, 0x002}, // May need to change, not sure how diff speak
        // PLL needs to be off until clock can be reduced
        {NAU_PWR1, 0x03D},
        {NAU_PWR2, 0x015},
        {NAU_PWR3, 0x06F},
    };

    uint8_t i;
    uint8_t result = 0;

    if (!nau88c22_write_reg(NAU_RESET, 0x000))
        goto init_exit;

    HAL_Delay(50);

    for (i = 0; i < sizeof(startup_seq) / sizeof(startup_seq[0]); i++)
    {
        if (!nau88c22_write_reg(startup_seq[i].reg, startup_seq[i].val))
            goto init_exit;
    }

    HAL_Delay(250);

    for (i = 0; i < sizeof(init_seq) / sizeof(init_seq[0]); i++)
    {
        if (!nau88c22_write_reg(init_seq[i].reg, init_seq[i].val))
            goto init_exit;
    }

    codec_initialized = 1;
    result = 1; // Success

init_exit:
    return result;
}

uint8_t nau88c22_sleep(uint8_t enable)
{
    if (!codec_initialized)
        return 0;

    uint16_t reg_data;
    uint8_t result = 0;

    if (!nau88c22_save_and_mute_all())
        return 0;

    if (!nau88c22_read_reg(NAU_PWR2, &reg_data))
        goto sleep_cleanup;

    if (enable)
        reg_data |= 0x040;
    else
        reg_data &= ~0x040;

    if (!nau88c22_write_reg(NAU_PWR2, reg_data))
        goto sleep_cleanup;

    result = 1;

sleep_cleanup:
    nau88c22_restore_mute_state();
    return result;
}

uint8_t nau88c22_hp_mic_toggle(uint8_t enable)
{
    if (!codec_initialized)
        return 0;

    uint8_t result = 0;
    uint16_t reg_value = enable ? 0x02A : 0x015;

    if (!nau88c22_save_and_mute_all())
        return 0;

    if (!nau88c22_write_reg(NAU_PWR2, reg_value))
        goto hp_mic_toggle_cleanup;

    result = 1;

hp_mic_toggle_cleanup:
    nau88c22_restore_mute_state();
    return result;
}

uint8_t nau88c22_hp_detect(void)
{
    return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5);
}

uint8_t nau88c22_read_volume_percent(uint8_t reg_addr, uint8_t *volume_percent)
{
    if (!codec_initialized || volume_percent == NULL)
        return 0;

    uint16_t reg_data;
    if (!nau88c22_read_reg(reg_addr, &reg_data))
    {
        return 0;
    }

    uint16_t current_volume_raw = reg_data & 0x3F;

    if (current_volume_raw == 0)
    {
        *volume_percent = 0;
    }
    else
    {
        *volume_percent = (uint8_t)(((uint32_t)current_volume_raw * 100 + 31) / 63);
        if (*volume_percent > 100)
        {
            *volume_percent = 100;
        }
    }

    return 1;
}

uint8_t nau88c22_set_output_volume(uint8_t volume, uint8_t left_reg, uint8_t right_reg)
{
    if (!codec_initialized)
        return 0;

    uint16_t vol_reg;
    uint8_t result = 0;

    if (!nau88c22_save_and_mute_all())
        return 0;

    if (volume == 0)
    {
        vol_reg = 0x000;
    }
    else
    {
        vol_reg = (volume * 63) / 100;
        vol_reg &= 0x3F;
    }

    uint16_t current_reg_data;
    if (!nau88c22_read_reg(left_reg, &current_reg_data))
        goto set_output_vol_cleanup;
    vol_reg |= (current_reg_data & 0x140);

    if (!nau88c22_write_reg(left_reg, vol_reg))
        goto set_output_vol_cleanup;

    if (!nau88c22_read_reg(right_reg, &current_reg_data))
        goto set_output_vol_cleanup;
    vol_reg |= (current_reg_data & 0x140);

    vol_reg |= 0x100;

    if (!nau88c22_write_reg(right_reg, vol_reg))
        goto set_output_vol_cleanup;

    result = 1;

set_output_vol_cleanup:
    nau88c22_restore_mute_state();
    return result;
}

uint8_t nau88c22_increment_output_volume(uint8_t increment)
{
    if (!codec_initialized)
        return 0;

    const uint8_t vol_step_percent = 5;
    uint8_t left_reg, right_reg;
    uint8_t current_volume_percent;
    uint8_t new_volume_percent;

    if (nau88c22_hp_detect())
    {
        left_reg = NAU_LHP_VOLUME;
        right_reg = NAU_RHP_VOLUME;
    }
    else
    {
        left_reg = NAU_LSPKOUT_VOLUME;
        right_reg = NAU_RSPKOUT_VOLUME;
    }

    if (!nau88c22_read_volume_percent(left_reg, &current_volume_percent))
        return 0;

    if (increment)
    {
        if (current_volume_percent > (100 - vol_step_percent))
        {
            new_volume_percent = 100;
        }
        else
        {
            new_volume_percent = current_volume_percent + vol_step_percent;
        }
    }
    else
    {
        if (current_volume_percent < vol_step_percent)
        {
            new_volume_percent = 0;
        }
        else
        {
            new_volume_percent = current_volume_percent - vol_step_percent;
        }
    }

    return nau88c22_set_output_volume(new_volume_percent, left_reg, right_reg);
}

uint8_t nau88c22_mute_output(uint8_t enable)
{
    if (!codec_initialized)
        return 0;

    uint16_t reg_data;
    const uint16_t mute_mask = (1 << 6);
    uint8_t result = 0;

    if (!nau88c22_save_and_mute_all())
        return 0;

    if (!nau88c22_read_reg(NAU_LHP_VOLUME, &reg_data))
        goto mute_output_cleanup;
    if (enable)
        reg_data |= mute_mask;
    else
        reg_data &= ~mute_mask;
    if (!nau88c22_write_reg(NAU_LHP_VOLUME, reg_data))
        goto mute_output_cleanup;

    if (!nau88c22_read_reg(NAU_RHP_VOLUME, &reg_data))
        goto mute_output_cleanup;
    if (enable)
        reg_data |= mute_mask;
    else
        reg_data &= ~mute_mask;
    reg_data |= 0x100;
    if (!nau88c22_write_reg(NAU_RHP_VOLUME, reg_data))
        goto mute_output_cleanup;

    if (!nau88c22_read_reg(NAU_LSPKOUT_VOLUME, &reg_data))
        goto mute_output_cleanup;
    if (enable)
        reg_data |= mute_mask;
    else
        reg_data &= ~mute_mask;
    if (!nau88c22_write_reg(NAU_LSPKOUT_VOLUME, reg_data))
        goto mute_output_cleanup;

    if (!nau88c22_read_reg(NAU_RSPKOUT_VOLUME, &reg_data))
        goto mute_output_cleanup;
    if (enable)
        reg_data |= mute_mask;
    else
        reg_data &= ~mute_mask;
    reg_data |= 0x100;
    if (!nau88c22_write_reg(NAU_RSPKOUT_VOLUME, reg_data))
        goto mute_output_cleanup;

    result = 1;

mute_output_cleanup:
    nau88c22_restore_mute_state();
    return result;
}

uint8_t nau88c22_mute_mic(uint8_t enable)
{
    if (!codec_initialized)
        return 0;

    uint16_t reg_data;
    const uint16_t mute_mask = (1 << 6);
    uint8_t result = 0;

    if (!nau88c22_save_and_mute_all())
        return 0;

    if (!nau88c22_read_reg(NAU_LEFT_INPUT_PGA_GAIN, &reg_data))
        goto mute_mic_cleanup;

    if (enable)
        reg_data |= mute_mask;
    else
        reg_data &= ~mute_mask;

    if (!nau88c22_write_reg(NAU_LEFT_INPUT_PGA_GAIN, reg_data))
        goto mute_mic_cleanup;

    result = 1;

mute_mic_cleanup:
    nau88c22_restore_mute_state();
    return result;
}

uint8_t nau88c22_mute_hp_mic(uint8_t enable)
{
    if (!codec_initialized)
        return 0;

    uint16_t reg_data;
    const uint16_t mute_mask = (1 << 6);
    uint8_t result = 0;

    if (!nau88c22_save_and_mute_all())
        return 0;

    if (!nau88c22_read_reg(NAU_RIGHT_INPUT_PGA_GAIN, &reg_data))
        goto mute_hp_mic_cleanup;

    if (enable)
        reg_data |= mute_mask;
    else
        reg_data &= ~mute_mask;

    if (!nau88c22_write_reg(NAU_RIGHT_INPUT_PGA_GAIN, reg_data))
        goto mute_hp_mic_cleanup;

    result = 1;

mute_hp_mic_cleanup:
    nau88c22_restore_mute_state();
    return result;
}

uint8_t nau88c22_set_mic_volume(uint8_t mic_channel, uint8_t volume)
{
    if (!codec_initialized)
        return 0;

    uint8_t reg_addr;
    uint8_t result = 0;

    if (mic_channel == MIC_BUILTIN)
    {
        reg_addr = NAU_LEFT_INPUT_PGA_GAIN;
    }
    else if (mic_channel == MIC_HEADPHONE)
    {
        reg_addr = NAU_RIGHT_INPUT_PGA_GAIN;
    }
    else
    {
        return 0;
    }

    if (!nau88c22_save_and_mute_all())
        return 0;

    uint16_t gain_reg_val;
    uint16_t current_reg_data;
    uint16_t new_reg_data;

    if (volume == 0)
    {
        gain_reg_val = 0x00;
    }
    else
    {
        gain_reg_val = (volume * 63) / 100;
        gain_reg_val &= 0x3F;
    }

    if (!nau88c22_read_reg(reg_addr, &current_reg_data))
        goto set_mic_vol_cleanup;

    new_reg_data = (current_reg_data & ~0x3F) | gain_reg_val;

    if (!nau88c22_write_reg(reg_addr, new_reg_data))
        goto set_mic_vol_cleanup;

    result = 1;

set_mic_vol_cleanup:
    nau88c22_restore_mute_state();
    return result;
}

uint8_t nau88c22_increment_mic_volume(uint8_t mic_channel, uint8_t increment)
{
    if (!codec_initialized)
        return 0;

    uint8_t reg_addr;
    if (mic_channel == MIC_BUILTIN)
    {
        reg_addr = NAU_LEFT_INPUT_PGA_GAIN;
    }
    else if (mic_channel == MIC_HEADPHONE)
    {
        reg_addr = NAU_RIGHT_INPUT_PGA_GAIN;
    }
    else
    {
        return 0;
    }

    const uint8_t vol_step_percent = 5;
    uint8_t current_volume_percent;
    uint8_t new_volume_percent;

    if (!nau88c22_read_volume_percent(reg_addr, &current_volume_percent))
        return 0;

    if (increment)
    {
        if (current_volume_percent > (100 - vol_step_percent))
        {
            new_volume_percent = 100;
        }
        else
        {
            new_volume_percent = current_volume_percent + vol_step_percent;
        }
    }
    else
    {
        if (current_volume_percent < vol_step_percent)
        {
            new_volume_percent = 0;
        }
        else
        {
            new_volume_percent = current_volume_percent - vol_step_percent;
        }
    }

    return nau88c22_set_mic_volume(mic_channel, new_volume_percent);
}

uint8_t nau88c22_mute_all(uint8_t enable)
{
    if (!codec_initialized)
        return 0;

    uint8_t success = 1;

    success &= nau88c22_mute_output(enable);
    success &= nau88c22_mute_mic(enable);
    success &= nau88c22_mute_hp_mic(enable);

    return success;
}

static uint8_t nau88c22_is_muted(uint8_t reg_addr)
{
    uint16_t reg_data;
    if (!nau88c22_read_reg(reg_addr, &reg_data))
    {
        return 0;
    }
    return (reg_data & (1 << 6)) ? 1 : 0;
}

uint8_t nau88c22_save_and_mute_all(void)
{
    if (!codec_initialized)
        return 0;

    if (mute_state_saved)
        return 0;

    saved_mute_state.output_muted = nau88c22_is_muted(NAU_LHP_VOLUME);
    saved_mute_state.mic_muted = nau88c22_is_muted(NAU_LEFT_INPUT_PGA_GAIN);
    saved_mute_state.hp_mic_muted = nau88c22_is_muted(NAU_RIGHT_INPUT_PGA_GAIN);

    mute_state_saved = 1;

    return nau88c22_mute_all(1);
}

uint8_t nau88c22_restore_mute_state(void)
{
    if (!codec_initialized || !mute_state_saved)
        return 0;

    uint8_t success = 1;

    success &= nau88c22_mute_output(saved_mute_state.output_muted);
    success &= nau88c22_mute_mic(saved_mute_state.mic_muted);
    success &= nau88c22_mute_hp_mic(saved_mute_state.hp_mic_muted);

    mute_state_saved = 0;

    return success;
}

uint8_t nau88c22_set_eq(uint8_t band, uint8_t gain)
{
    if (!codec_initialized || band > 4)
        return 0;

    uint16_t gain_val = gain & 0x1F;
    uint8_t eq_reg;
    uint8_t result = 0;

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

    uint16_t current_reg_data;
    uint16_t new_reg_data;

    if (!nau88c22_save_and_mute_all())
        return 0;

    if (!nau88c22_read_reg(eq_reg, &current_reg_data))
        goto set_eq_cleanup;

    new_reg_data = (current_reg_data & ~0x1F) | gain_val;

    if (!nau88c22_write_reg(eq_reg, new_reg_data))
        goto set_eq_cleanup;

    result = 1;

set_eq_cleanup:
    nau88c22_restore_mute_state();
    return result;
}
