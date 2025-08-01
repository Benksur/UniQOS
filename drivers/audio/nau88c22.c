#include "nau88c22.h"
#include "errornum.h"

static uint8_t nau88c22_is_muted(uint8_t reg_addr);
static uint8_t nau88c22_save_and_mute_all(nau88c22_codec_t *codec);
static uint8_t nau88c22_restore_mute_state(nau88c22_codec_t *codec);

// uint8_t nau88c22_write_reg(uint8_t reg_addr, uint16_t reg_data)
// {
//     HAL_StatusTypeDef status;

//     if (reg_data == NULL || reg_addr > LSM6DSV_FIFO_DATA_OUT_Z_H)
//     {
//         return EINVAL;
//     }

//     status = HAL_I2C_Mem_Write(&IMU_I2C_HANDLE, NAU88C22_I2C_ADDR << 1, reg_addr, I2C_MEMADD_SIZE_8BIT, reg_data, 2, 1000);

//     if (status != HAL_OK)
//     {
//         return EIO;
//     }

//     return 0;
// }

uint8_t nau88c22_write_reg(uint8_t reg_addr, uint16_t reg_data)
{
    uint8_t data[2];
    data[0] = (reg_addr << 1) | ((reg_data >> 8) & 0x01);
    data[1] = reg_data & 0xFF;

    if (HAL_I2C_Master_Transmit(&AUDIO_I2C_HANDLE, NAU88C22_I2C_ADDR << 1, data, 2, 100) != HAL_OK)
    {
        return EIO;
    }

    return 0; 
}

uint8_t nau88c22_read_reg(uint8_t reg_addr, uint16_t *reg_data)
{
    uint8_t addr_byte;
    uint8_t data[2] = {0, 0};
    HAL_StatusTypeDef status;

    if (reg_data == NULL)
    {
        return EINVAL;
    }

    addr_byte = reg_addr << 1;

    status = HAL_I2C_Master_Transmit(&AUDIO_I2C_HANDLE, NAU88C22_I2C_ADDR << 1, &addr_byte, 1, 100);
    if (status != HAL_OK)
    {
        return EIO;
    }

    status = HAL_I2C_Master_Receive(&AUDIO_I2C_HANDLE, NAU88C22_I2C_ADDR << 1, data, 2, 100);
    if (status != HAL_OK)
    {
        return EIO;
    }
    *reg_data = ((data[0] & 0x01) << 8) | data[1];
    return 0; 
}

uint8_t nau88c22_enable_lin_mic(nau88c22_codec_t *codec) {
    typedef struct
    {
        uint8_t reg;
        uint16_t val;
    } reg_conf_t;

    const reg_conf_t write_seq[] = {
        {NAU_PWR2, 0x1BF},
        {NAU_INPUT_CONTROL, 0x0C4},
        {NAU_COMPANDING, 0x001},
        {NAU_LEFT_ADC_BOOST, 0x177},
        {NAU_RIGHT_ADC_BOOST, 0x177},
        
    };

    uint8_t i;

    for (i = 0; i < sizeof(write_seq) / sizeof(write_seq[0]); i++)
    {
        if (nau88c22_write_reg(write_seq[i].reg, write_seq[i].val) != 0)
            return EIO;
    }
    return 0;
}

uint8_t nau88c22_init(nau88c22_codec_t *codec)
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
        {NAU_AINTF, 0x051}, // mono, 24 bit audio stream
        {NAU_CLOCK2, 0x001},
        // {NAU_JACK_DETECT1, 0x040}, // disable now because jack detection is shit
        // {NAU_JACK_DETECT2, 0x021}, // toggle loudspeaker to headphones on jack detect
        // R = 9.408163070, f2 = 90.318MHz, f1 = 19.2/2 = 9.6MHz
        {NAU_PPL_N, 0x019},  // N of 9
        {NAU_PPL_K1, 0x1A},  // 0.408163070
        {NAU_PPL_K2, 0x039}, // 0.000000000
        {NAU_PPL_K3, 0x0B0},
        {NAU_INPUT_CONTROL, 0x0B3}, // 0.65x microphone bias
        {NAU_OUTPUT_CONTROL, 0x002}, // May need to change, not sure how diff speak
        // PLL needs to be off until clock can be reduced
        {NAU_PWR1, 0x03D},
        {NAU_PWR2, 0x195}, // enable headphone drivers
        {NAU_PWR3, 0x06F},

    };

    uint8_t i;
    uint8_t err = 0;

    if (nau88c22_write_reg(NAU_RESET, 0x000) != 0)
        return EIO;

    HAL_Delay(50);

    for (i = 0; i < sizeof(startup_seq) / sizeof(startup_seq[0]); i++)
    {
        if (nau88c22_write_reg(startup_seq[i].reg, startup_seq[i].val) != 0)
            return EIO;
    }

    HAL_Delay(250);

    for (i = 0; i < sizeof(init_seq) / sizeof(init_seq[0]); i++)
    {
        if (nau88c22_write_reg(init_seq[i].reg, init_seq[i].val) != 0)
            return EIO;
    }

    codec->initialized = 1;
    codec->volume = 0;

    return 0;
}

uint8_t nau88c22_sleep(nau88c22_codec_t *codec, uint8_t enable)
{
    if (!codec || !codec->initialized)
        return ENODEV;

    uint16_t reg_data;
    uint8_t err = 0;

    err = nau88c22_save_and_mute_all(codec);
    if (err != 0)
        return err;

    err = nau88c22_read_reg(NAU_PWR2, &reg_data);
    if (err != 0)
    {
        nau88c22_restore_mute_state(codec); 
        return EIO;
    }

    if (enable)
        reg_data |= 0x040;
    else
        reg_data &= ~0x040;

    err = nau88c22_write_reg(NAU_PWR2, reg_data);
    if (err != 0)
    {
        nau88c22_restore_mute_state(codec);
        return EIO;
    }

    return nau88c22_restore_mute_state(codec);
}

uint8_t nau88c22_hp_mic_toggle(nau88c22_codec_t *codec, uint8_t enable)
{
    if (!codec || !codec->initialized)
        return ENODEV;

    uint8_t err = 0;
    uint16_t reg_value = enable ? 0x02A : 0x015;

    err = nau88c22_save_and_mute_all(codec);
    if (err != 0)
        return err;

    err = nau88c22_write_reg(NAU_PWR2, reg_value);
    if (err != 0)
    {
        nau88c22_restore_mute_state(codec);
        return EIO;
    }

    return nau88c22_restore_mute_state(codec);
}

uint8_t nau88c22_hp_detect(void)
{
    return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5);
}

uint8_t nau88c22_read_volume_percent(nau88c22_codec_t *codec, uint8_t reg_addr, uint8_t *volume_percent)
{
    if (!codec || !codec->initialized)
        return ENODEV;
    if (volume_percent == NULL)
        return EINVAL;

    uint16_t reg_data;
    uint8_t err = nau88c22_read_reg(reg_addr, &reg_data);
    if (err != 0)
    {
        return EIO;
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

    return 0;
}

uint8_t nau88c22_set_output_volume(nau88c22_codec_t *codec, uint8_t volume, uint8_t left_reg, uint8_t right_reg)
{
    if (!codec || !codec->initialized)
        return ENODEV;

    uint16_t vol_reg;
    uint8_t err = 0;
    uint16_t current_reg_data;

    err = nau88c22_save_and_mute_all(codec);
    if (err != 0)
        return err;

    if (volume == 0)
    {
        vol_reg = 0x000;
    }
    else
    {
        if (volume > 100) volume = 100;
        vol_reg = (volume * 63) / 100;
        vol_reg &= 0x3F;
    }

    err = nau88c22_read_reg(left_reg, &current_reg_data);
    if (err != 0) { goto set_output_vol_cleanup_err; }
    vol_reg |= (current_reg_data & 0x140);
    err = nau88c22_write_reg(left_reg, vol_reg);
    if (err != 0) { goto set_output_vol_cleanup_err; }

    err = nau88c22_read_reg(right_reg, &current_reg_data);
    if (err != 0) { goto set_output_vol_cleanup_err; }
    vol_reg |= (current_reg_data & 0x140);
    vol_reg |= 0x100;
    err = nau88c22_write_reg(right_reg, vol_reg);
    if (err != 0) { goto set_output_vol_cleanup_err; }

    codec->volume = volume;

    return nau88c22_restore_mute_state(codec);

set_output_vol_cleanup_err:
    nau88c22_restore_mute_state(codec);
    return EIO;
}

uint8_t nau88c22_set_output_volume_simple(nau88c22_codec_t *codec, uint8_t volume, uint8_t left_reg, uint8_t right_reg)
{
    if (!codec || !codec->initialized)
        return ENODEV;

    uint16_t vol_reg;
    uint8_t err = 0;
    uint16_t current_reg_data;

    if (volume == 0)
    {
        vol_reg = 0x000;
    }
    else
    {
        if (volume > 100) volume = 100;
        vol_reg = (volume * 63) / 100;
        vol_reg &= 0x3F;
    }

    err = nau88c22_read_reg(left_reg, &current_reg_data);
    if (err != 0) return EIO;
    vol_reg |= (current_reg_data & 0x140);
    err = nau88c22_write_reg(left_reg, vol_reg);
    if (err != 0) return EIO;

    err = nau88c22_read_reg(right_reg, &current_reg_data);
    if (err != 0) return EIO;
    vol_reg |= (current_reg_data & 0x140);
    vol_reg |= 0x100;
    err = nau88c22_write_reg(right_reg, vol_reg);
    if (err != 0) return EIO;

    codec->volume = volume;
    return 0;
}

uint8_t nau88c22_increment_output_volume(nau88c22_codec_t *codec, uint8_t increment)
{
    if (!codec || !codec->initialized)
        return ENODEV;

    const uint8_t vol_step_percent = 5;
    uint8_t left_reg, right_reg;
    uint8_t current_volume;
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

    current_volume = nau88c22_get_output_volume(codec);


    if (increment)
    {
        if (current_volume >= (100 - vol_step_percent))
        {
            new_volume_percent = 100;
        }
        else
        {
            new_volume_percent = current_volume + vol_step_percent;
        }
    }
    else
    {
        if (current_volume <= vol_step_percent)
        {
            new_volume_percent = 0;
        }
        else
        {
            new_volume_percent = current_volume - vol_step_percent;
        }
    }

    return nau88c22_set_output_volume(codec, new_volume_percent, left_reg, right_reg);
}

uint8_t nau88c22_mute_output(nau88c22_codec_t *codec, uint8_t enable)
{
    if (!codec || !codec->initialized)
        return ENODEV;

    uint16_t reg_data;
    const uint16_t mute_mask = (1 << 6);
    uint8_t err = 0;

    err = nau88c22_read_reg(NAU_LHP_VOLUME, &reg_data); if (err != 0) return EIO;
    if (enable) reg_data |= mute_mask; else reg_data &= ~mute_mask;
    err = nau88c22_write_reg(NAU_LHP_VOLUME, reg_data); if (err != 0) return EIO;

    err = nau88c22_read_reg(NAU_RHP_VOLUME, &reg_data); if (err != 0) return EIO;
    if (enable) reg_data |= mute_mask; else reg_data &= ~mute_mask;
    reg_data |= 0x100; 
    err = nau88c22_write_reg(NAU_RHP_VOLUME, reg_data); if (err != 0) return EIO;

    err = nau88c22_read_reg(NAU_LSPKOUT_VOLUME, &reg_data); if (err != 0) return EIO;
    if (enable) reg_data |= mute_mask; else reg_data &= ~mute_mask;
    err = nau88c22_write_reg(NAU_LSPKOUT_VOLUME, reg_data); if (err != 0) return EIO;

    err = nau88c22_read_reg(NAU_RSPKOUT_VOLUME, &reg_data); if (err != 0) return EIO;
    if (enable) reg_data |= mute_mask; else reg_data &= ~mute_mask;
    reg_data |= 0x100;
    err = nau88c22_write_reg(NAU_RSPKOUT_VOLUME, reg_data); if (err != 0) return EIO;

    return 0;
}

uint8_t nau88c22_mute_mic(nau88c22_codec_t *codec, uint8_t enable)
{
    if (!codec || !codec->initialized)
        return ENODEV;

    uint16_t reg_data;
    const uint16_t mute_mask = (1 << 6);
    uint8_t err = 0;

    err = nau88c22_read_reg(NAU_LEFT_INPUT_PGA_GAIN, &reg_data);
    if (err != 0) return EIO;

    if (enable)
        reg_data |= mute_mask;
    else
        reg_data &= ~mute_mask;

    err = nau88c22_write_reg(NAU_LEFT_INPUT_PGA_GAIN, reg_data);
    if (err != 0) return EIO;

    return 0;
}

uint8_t nau88c22_mute_hp_mic(nau88c22_codec_t *codec, uint8_t enable)
{
    if (!codec || !codec->initialized)
        return ENODEV;

    uint16_t reg_data;
    const uint16_t mute_mask = (1 << 6);
    uint8_t err = 0;

    err = nau88c22_read_reg(NAU_RIGHT_INPUT_PGA_GAIN, &reg_data);
    if (err != 0) return EIO;

    if (enable)
        reg_data |= mute_mask;
    else
        reg_data &= ~mute_mask;

    err = nau88c22_write_reg(NAU_RIGHT_INPUT_PGA_GAIN, reg_data);
    if (err != 0) return EIO;

    return 0;
}

uint8_t nau88c22_set_mic_volume(nau88c22_codec_t *codec, uint8_t mic_channel, uint8_t volume)
{
    if (!codec || !codec->initialized)
        return ENODEV;

    uint8_t reg_addr;
    uint8_t err = 0;

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
        return EINVAL;
    }

    err = nau88c22_save_and_mute_all(codec);
    if (err != 0)
        return err;

    uint16_t gain_reg_val;
    uint16_t current_reg_data;
    uint16_t new_reg_data;

    if (volume == 0)
    {
        gain_reg_val = 0x00;
    }
    else
    {
        if (volume > 100) volume = 100;
        gain_reg_val = (volume * 63) / 100;
        gain_reg_val &= 0x3F;
    }

    err = nau88c22_read_reg(reg_addr, &current_reg_data);
    if (err != 0) goto set_mic_vol_cleanup_err;

    new_reg_data = (current_reg_data & ~0x3F) | gain_reg_val;

    err = nau88c22_write_reg(reg_addr, new_reg_data);
    if (err != 0) goto set_mic_vol_cleanup_err;

    return nau88c22_restore_mute_state(codec);

set_mic_vol_cleanup_err:
    nau88c22_restore_mute_state(codec);
    return EIO;
}

uint8_t nau88c22_increment_mic_volume(nau88c22_codec_t *codec, uint8_t mic_channel, uint8_t increment)
{
    if (!codec || !codec->initialized)
        return ENODEV;

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
        return EINVAL;
    }

    const uint8_t vol_step_percent = 5;
    uint8_t current_volume;
    uint8_t new_volume_percent;
    uint8_t err;

    err = nau88c22_read_volume_percent(codec, reg_addr, &current_volume);
    if (err != 0)
        return err;

    if (increment)
    {
        if (current_volume >= (100 - vol_step_percent))
        {
            new_volume_percent = 100;
        }
        else
        {
            new_volume_percent = current_volume + vol_step_percent;
        }
    }
    else
    {
        if (current_volume <= vol_step_percent)
        {
            new_volume_percent = 0;
        }
        else
        {
            new_volume_percent = current_volume - vol_step_percent;
        }
    }

    return nau88c22_set_mic_volume(codec, mic_channel, new_volume_percent);
}

uint8_t nau88c22_mute_all(nau88c22_codec_t *codec, uint8_t enable)
{
    if (!codec || !codec->initialized)
        return ENODEV;

    uint8_t err = 0;
    uint8_t final_err = 0;
    
    enable = (enable != 0) ? 1 : 0;

    err = nau88c22_mute_output(codec, enable);
    if (err != 0) {
        final_err = err;
    }

    err = nau88c22_mute_mic(codec, enable);
    if (err != 0 && final_err == 0) {
        final_err = err;
    }

    err = nau88c22_mute_hp_mic(codec, enable);
    if (err != 0 && final_err == 0) {
        final_err = err;
    }

    return final_err;
}

static uint8_t nau88c22_is_muted(uint8_t reg_addr)
{
    uint16_t reg_data;
    uint8_t ret;
    
    if (nau88c22_read_reg(reg_addr, &reg_data) != 0)
    {
        return 0;
    }
    
    ret = (reg_data & (1 << 6)) ? 1 : 0;
    return ret;
}

static uint8_t nau88c22_save_and_mute_all(nau88c22_codec_t *codec)
{
    if (!codec || !codec->initialized)
        return ENODEV;

    uint8_t output_muted = nau88c22_is_muted(NAU_LHP_VOLUME);
    uint8_t mic_muted = nau88c22_is_muted(NAU_LEFT_INPUT_PGA_GAIN);
    uint8_t hp_mic_muted = nau88c22_is_muted(NAU_RIGHT_INPUT_PGA_GAIN);
    
    codec->saved_mute_state.output_muted = (output_muted == 1) ? 1 : 0;
    codec->saved_mute_state.mic_muted = (mic_muted == 1) ? 1 : 0;
    codec->saved_mute_state.hp_mic_muted = (hp_mic_muted == 1) ? 1 : 0;

    uint8_t mute_result = nau88c22_mute_all(codec, 1);

    return mute_result;
}

static uint8_t nau88c22_restore_mute_state(nau88c22_codec_t *codec)
{
    if (!codec || !codec->initialized)
        return ENODEV;


    uint8_t err = 0;
    uint8_t final_err = 0;
    
    // Validate mute values before restoring
    uint8_t output_muted = (codec->saved_mute_state.output_muted == 1) ? 1 : 0;
    uint8_t mic_muted = (codec->saved_mute_state.mic_muted == 1) ? 1 : 0;
    uint8_t hp_mic_muted = (codec->saved_mute_state.hp_mic_muted == 1) ? 1 : 0;

    err = nau88c22_mute_output(codec, output_muted);
    if (err != 0 && final_err == 0) final_err = err;

    err = nau88c22_mute_mic(codec, mic_muted);
    if (err != 0 && final_err == 0) final_err = err;

    err = nau88c22_mute_hp_mic(codec, hp_mic_muted);
    if (err != 0 && final_err == 0) final_err = err;

    return final_err;
}

uint8_t nau88c22_set_eq(nau88c22_codec_t *codec, uint8_t band, uint8_t gain)
{
    if (!codec || !codec->initialized)
        return ENODEV;
    if (band > 4)
        return EINVAL;

    uint16_t gain_val = gain & 0x1F;
    uint8_t eq_reg;
    uint8_t err = 0;

    switch (band)
    {
    case 0: eq_reg = NAU_EQ1; break;
    case 1: eq_reg = NAU_EQ2; break;
    case 2: eq_reg = NAU_EQ3; break;
    case 3: eq_reg = NAU_EQ4; break;
    case 4: eq_reg = NAU_EQ5; break;
    default: return EINVAL;
    }

    err = nau88c22_save_and_mute_all(codec);
    if (err != 0)
        return err;

    uint16_t current_reg_data;
    uint16_t new_reg_data;

    err = nau88c22_read_reg(eq_reg, &current_reg_data);
    if (err != 0) goto set_eq_cleanup_err;

    new_reg_data = (current_reg_data & ~0x1F) | gain_val;

    err = nau88c22_write_reg(eq_reg, new_reg_data);
    if (err != 0) goto set_eq_cleanup_err;

    return nau88c22_restore_mute_state(codec);

set_eq_cleanup_err:
    nau88c22_restore_mute_state(codec);
    return EIO;
}

uint8_t nau88c22_get_output_volume(nau88c22_codec_t *codec)
{
    if (!codec || !codec->initialized)
        return 0;

    uint8_t reg_addr;
    uint16_t reg_data;
    uint8_t err;

    // Determine which register to read based on headphone detection
    if (nau88c22_hp_detect())
    {
        reg_addr = NAU_LHP_VOLUME;
    }
    else
    {
        reg_addr = NAU_LSPKOUT_VOLUME;
    }

    err = nau88c22_read_reg(reg_addr, &reg_data);
    if (err != 0)
        return codec->volume;  // Fall back to cached value if read fails

    // Extract volume bits (0-63) and convert to percentage
    uint8_t raw_volume = reg_data & 0x3F;
    if (raw_volume == 0)
        return 0;
    
    uint8_t volume_percent = (uint8_t)(((uint32_t)raw_volume * 100 + 31) / 63);
    if (volume_percent > 100)
        volume_percent = 100;

    // Update cached value
    codec->volume = volume_percent;
    return volume_percent;
}
