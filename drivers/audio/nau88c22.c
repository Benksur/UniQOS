/**
 * @file nau88c22.c
 * @brief NAU88C22 audio codec driver implementation
 *
 * This driver provides I2C communication and audio control functions for the NAU88C22
 * audio codec. It supports speaker, headphone, and microphone control with volume
 * and mute functionality.
 */

#include "nau88c22.h"
#include "errornum.h"


static uint8_t nau88c22_write_reg(uint8_t reg_addr, uint16_t reg_data);
static uint8_t nau88c22_read_reg(uint8_t reg_addr, uint16_t *reg_data);
static uint8_t nau88c22_enable_lin_mic(void);
static uint8_t nau88c22_init(void);
static uint8_t nau88c22_mute_reg(uint8_t reg_addr, uint8_t enable, uint8_t add_0x100);
static uint8_t nau88c22_mute_lr(uint8_t left_reg, uint8_t right_reg, uint8_t enable);
static uint8_t nau88c22_mute_speaker(uint8_t enable);
static uint8_t nau88c22_mute_hp(uint8_t enable);
static uint8_t nau88c22_mute_mic(uint8_t enable);
static uint8_t nau88c22_mute_hp_mic(uint8_t enable);
static void nau88c22_mute_all(uint8_t enable);
static uint8_t nau88c22_is_muted(uint8_t reg_addr);
static uint8_t nau88c22_sleep(uint8_t enable);
static uint8_t nau88c22_hp_mic_toggle(uint8_t enable);
static uint8_t nau88c22_hp_detect(void);
static uint8_t nau88c22_read_volume_percent(uint8_t reg_addr, uint8_t *volume_percent);
static uint8_t nau88c22_set_volume_reg(uint8_t reg_addr, uint8_t volume, uint8_t add_0x100);
static uint8_t nau88c22_set_volume_lr(uint8_t left_reg, uint8_t right_reg, uint8_t volume);
static uint8_t nau88c22_set_output_volume(uint8_t volume, uint8_t reg_addr);
static uint8_t nau88c22_set_output_volume_lr(uint8_t volume, uint8_t left_reg, uint8_t right_reg);
static uint8_t nau88c22_set_mic_volume(uint8_t volume);
static uint8_t nau88c22_set_hp_mic_volume(uint8_t volume);
static uint8_t nau88c22_set_speaker_volume(uint8_t volume);
static uint8_t nau88c22_set_hp_volume(uint8_t volume);
static uint8_t nau88c22_set_eq(uint8_t band, uint8_t gain);

// Vtable-compatible wrappers
static void nau88c22_init_void(void) { (void)nau88c22_init(); }
static void nau88c22_sleep_void(uint8_t enable) { (void)nau88c22_sleep(enable); }
static void nau88c22_mute_speaker_void(uint8_t enable) { (void)nau88c22_mute_speaker(enable); }
static void nau88c22_set_speaker_volume_void(uint8_t volume) { (void)nau88c22_set_speaker_volume(volume); }
static void nau88c22_mute_mic_void(uint8_t enable) { (void)nau88c22_mute_mic(enable); }
static void nau88c22_set_mic_volume_void(uint8_t volume) { (void)nau88c22_set_mic_volume(volume); }
static void nau88c22_mute_hp_void(uint8_t enable) { (void)nau88c22_mute_hp(enable); }
static void nau88c22_set_hp_volume_void(uint8_t volume) { (void)nau88c22_set_hp_volume(volume); }
static void nau88c22_mute_hp_mic_void(uint8_t enable) { (void)nau88c22_mute_hp_mic(enable); }
static void nau88c22_set_hp_mic_volume_void(uint8_t volume) { (void)nau88c22_set_hp_mic_volume(volume); }

/**
 * @brief Write a 16-bit register value to the NAU88C22 codec via I2C
 * @param reg_addr Register address to write to
 * @param reg_data 16-bit data value to write
 * @return 0 on success, EIO on I2C communication error
 */
static uint8_t nau88c22_write_reg(uint8_t reg_addr, uint16_t reg_data)
{
    uint8_t data[2];
    data[0] = (reg_addr << 1) | ((reg_data >> 8) & 0x01); // pack reg addr and msb
    data[1] = reg_data & 0xFF;                            // lsb

    if (HAL_I2C_Master_Transmit(&AUDIO_I2C_HANDLE, NAU88C22_I2C_ADDR << 1, data, 2, 100) != HAL_OK)
    {
        return EIO;
    }

    return 0;
}

/**
 * @brief Read a 16-bit register value from the NAU88C22 codec via I2C
 * @param reg_addr Register address to read from
 * @param reg_data Pointer to store the 16-bit data value
 * @return 0 on success, EINVAL if reg_data is NULL, EIO on I2C communication error
 */
static uint8_t nau88c22_read_reg(uint8_t reg_addr, uint16_t *reg_data)
{
    uint8_t data[2] = {0, 0};
    HAL_StatusTypeDef status;

    if (reg_data == NULL)
    {
        return EINVAL;
    }

    status = HAL_I2C_Mem_Read(&AUDIO_I2C_HANDLE, NAU88C22_I2C_ADDR << 1, reg_addr << 1, I2C_MEMADD_SIZE_8BIT, data, 2, 100);
    if (status != HAL_OK)
    {
        return EIO;
    }

    *reg_data = ((data[0] & 0x01) << 8) | data[1]; // reconstruct 16-bit value

    return 0;
}

/**
 * @brief Initialize the NAU88C22 codec with startup and configuration sequences
 * @return 0 on success, EIO on I2C communication error
 */
static uint8_t nau88c22_init(void)
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
        // for 44.1KHz sample rate
        // R = 9.408163070, f2 = 90.318MHz, f1 = 19.2/2 = 9.6MHz
        {NAU_PPL_N, 0x019}, // N of 9
        // fractional portion is 0x6872B0
        {NAU_PPL_K1, 0x1A},          // highest 6 bits of fractional portion
        {NAU_PPL_K2, 0x039},         // middle 9 bits of fractional portion
        {NAU_PPL_K3, 0x0B0},         // lowest 9 bits of fractional portion
        {NAU_INPUT_CONTROL, 0x0B3},  // 0.65x microphone bias
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

    return 0;
}

/**
 * @brief Enable linear microphone input with proper gain settings
 * @return 0 on success, EIO on I2C communication error
 */
static uint8_t nau88c22_enable_lin_mic(void)
{
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

/**
 * @brief General-purpose mute helper for a single register
 * @param reg_addr Register address to modify
 * @param enable 1 to mute, 0 to unmute
 * @param add_0x100 1 to add 0x100 offset for right channel, 0 for left channel
 * @return 0 on success, EIO on I2C communication error
 */
static uint8_t nau88c22_mute_reg(uint8_t reg_addr, uint8_t enable, uint8_t add_0x100)
{
    uint16_t reg_data;
    const uint16_t mute_mask = (1 << 6); // bit 6 is mute bit
    uint8_t err = nau88c22_read_reg(reg_addr, &reg_data);
    if (err != 0)
        return EIO;
    if (enable)
        reg_data |= mute_mask; // set mute bit
    else
        reg_data &= ~mute_mask; // clear mute bit
    if (add_0x100)
        reg_data |= 0x100; // add 0x100 for right channel
    err = nau88c22_write_reg(reg_addr, reg_data);
    if (err != 0)
        return EIO;
    return 0;
}

/**
 * @brief General-purpose mute helper for left/right register pair
 * @param left_reg Left channel register address
 * @param right_reg Right channel register address
 * @param enable 1 to mute, 0 to unmute
 * @return 0 on success, EIO on I2C communication error
 */
static uint8_t nau88c22_mute_lr(uint8_t left_reg, uint8_t right_reg, uint8_t enable)
{
    uint8_t err;
    err = nau88c22_mute_reg(left_reg, enable, 0); // left channel
    if (err != 0)
        return err;
    err = nau88c22_mute_reg(right_reg, enable, 1); // right channel
    if (err != 0)
        return err;
    return 0;
}

/**
 * @brief Mute or unmute speaker output
 * @param enable 1 to mute, 0 to unmute
 * @return 0 on success, EIO on I2C communication error
 */
static uint8_t nau88c22_mute_speaker(uint8_t enable)
{
    return nau88c22_mute_lr(NAU_LSPKOUT_VOLUME, NAU_RSPKOUT_VOLUME, enable);
}

/**
 * @brief Mute or unmute headphone output
 * @param enable 1 to mute, 0 to unmute
 * @return 0 on success, EIO on I2C communication error
 */
static uint8_t nau88c22_mute_hp(uint8_t enable)
{
    return nau88c22_mute_lr(NAU_LHP_VOLUME, NAU_RHP_VOLUME, enable);
}

/**
 * @brief Mute or unmute microphone input
 * @param enable 1 to mute, 0 to unmute
 * @return 0 on success, EIO on I2C communication error
 */
static uint8_t nau88c22_mute_mic(uint8_t enable)
{
    return nau88c22_mute_reg(NAU_LEFT_INPUT_PGA_GAIN, enable, 0);
}

/**
 * @brief Mute or unmute headphone microphone input
 * @param enable 1 to mute, 0 to unmute
 * @return 0 on success, EIO on I2C communication error
 */
static uint8_t nau88c22_mute_hp_mic(uint8_t enable)
{
    return nau88c22_mute_reg(NAU_RIGHT_INPUT_PGA_GAIN, enable, 0);
}

/**
 * @brief Mute or unmute all audio outputs and inputs
 * @param enable 1 to mute all, 0 to unmute all
 */
static void nau88c22_mute_all(uint8_t enable)
{
    nau88c22_mute_speaker(enable);
    nau88c22_mute_hp(enable);
    nau88c22_mute_mic(enable);
    nau88c22_mute_hp_mic(enable);
}

/**
 * @brief Check if a register is currently muted
 * @param reg_addr Register address to check
 * @return 1 if muted, 0 if not muted or on error
 */
static uint8_t nau88c22_is_muted(uint8_t reg_addr)
{
    uint16_t reg_data;
    uint8_t ret;

    if (nau88c22_read_reg(reg_addr, &reg_data) != 0)
    {
        return 0;
    }

    ret = (reg_data & (1 << 6)) ? 1 : 0; // check mute bit
    return ret;
}

/**
 * @brief Enable or disable sleep mode
 * @param enable 1 to enable sleep, 0 to disable sleep
 * @return 0 on success, EIO on I2C communication error
 */
static uint8_t nau88c22_sleep(uint8_t enable)
{
    uint16_t reg_data;
    uint8_t err = 0;

    err = nau88c22_read_reg(NAU_PWR2, &reg_data);
    if (err != 0)
        return EIO;

    if (enable)
        reg_data |= 0x040; // set sleep bit
    else
        reg_data &= ~0x040; // clear sleep bit

    err = nau88c22_write_reg(NAU_PWR2, reg_data);
    if (err != 0)
        return EIO;

    return 0;
}

/**
 * @brief Toggle headphone microphone routing
 * @param enable 1 to enable hp mic, 0 to disable
 * @return 0 on success, EIO on I2C communication error
 */
static uint8_t nau88c22_hp_mic_toggle(uint8_t enable)
{
    uint8_t err = 0;
    uint16_t reg_value = enable ? 0x02A : 0x015;

    err = nau88c22_write_reg(NAU_PWR2, reg_value);
    if (err != 0)
        return EIO;

    return 0;
}

/**
 * @brief Detect if headphones are connected
 * @return GPIO pin state (1 if connected, 0 if not)
 */
static uint8_t nau88c22_hp_detect(void)
{
    return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5);
}

/**
 * @brief Read volume percentage from a register
 * @param reg_addr Register address to read from
 * @param volume_percent Pointer to store volume percentage (0-100)
 * @return 0 on success, EINVAL if volume_percent is NULL, EIO on I2C error
 */
static uint8_t nau88c22_read_volume_percent(uint8_t reg_addr, uint8_t *volume_percent)
{
    if (volume_percent == NULL)
        return EINVAL;

    uint16_t reg_data;
    uint8_t err = nau88c22_read_reg(reg_addr, &reg_data);
    if (err != 0)
    {
        return EIO;
    }

    uint16_t current_volume_raw = reg_data & 0x3F; // extract 6-bit volume

    if (current_volume_raw == 0)
    {
        *volume_percent = 0;
    }
    else
    {
        *volume_percent = (uint8_t)(((uint32_t)current_volume_raw * 100 + 31) / 63); // convert to percentage
        if (*volume_percent > 100)
        {
            *volume_percent = 100;
        }
    }

    return 0;
}

/**
 * @brief General-purpose set volume helper for a single register
 * @param reg_addr Register address to modify
 * @param volume Volume level (0-100)
 * @param add_0x100 1 to add 0x100 offset for right channel, 0 for left channel
 * @return 0 on success, EIO on I2C communication error
 */
static uint8_t nau88c22_set_volume_reg(uint8_t reg_addr, uint8_t volume, uint8_t add_0x100)
{
    uint16_t reg_data;
    uint8_t err = nau88c22_read_reg(reg_addr, &reg_data);
    if (err != 0)
        return EIO;
    uint16_t vol_val;
    if (volume == 0)
    {
        vol_val = 0x000;
    }
    else
    {
        if (volume > 100)
            volume = 100;
        vol_val = (volume * 63) / 100; // convert percentage to 6-bit value
        vol_val &= 0x3F;
    }
    reg_data = (reg_data & ~0x3F) | vol_val; // update volume bits
    if (add_0x100)
        reg_data |= 0x100; // add 0x100 for right channel
    err = nau88c22_write_reg(reg_addr, reg_data);
    if (err != 0)
        return EIO;
    return 0;
}

/**
 * @brief General-purpose set volume helper for left/right register pair
 * @param left_reg Left channel register address
 * @param right_reg Right channel register address
 * @param volume Volume level (0-100)
 * @return 0 on success, EIO on I2C communication error
 */
static uint8_t nau88c22_set_volume_lr(uint8_t left_reg, uint8_t right_reg, uint8_t volume)
{
    uint8_t err;
    err = nau88c22_set_volume_reg(left_reg, volume, 0); // left channel
    if (err != 0)
        return err;
    err = nau88c22_set_volume_reg(right_reg, volume, 1); // right channel
    if (err != 0)
        return err;
    return 0;
}

/**
 * @brief Set output volume for a single register
 * @param volume Volume level (0-100)
 * @param reg_addr Register address to modify
 * @return 0 on success, EIO on I2C communication error
 */
static uint8_t nau88c22_set_output_volume(uint8_t volume, uint8_t reg_addr)
{
    // direct volume setting without mute/unmute cycle
    // the mute/unmute cycle was causing volume corruption
    return nau88c22_set_volume_reg(reg_addr, volume, 0);
}

/**
 * @brief Set output volume for left/right register pair
 * @param volume Volume level (0-100)
 * @param left_reg Left channel register address
 * @param right_reg Right channel register address
 * @return 0 on success, EIO on I2C communication error
 */
static uint8_t nau88c22_set_output_volume_lr(uint8_t volume, uint8_t left_reg, uint8_t right_reg)
{
    // direct volume setting without mute/unmute cycle for all outputs
    // the mute/unmute cycle was causing volume corruption
    uint8_t err;
    err = nau88c22_set_volume_reg(left_reg, volume, 0);
    if (err != 0)
        return err;
    err = nau88c22_set_volume_reg(right_reg, volume, 1);
    if (err != 0)
        return err;
    return 0;
}

/**
 * @brief Set microphone volume
 * @param volume Volume level (0-100)
 * @return 0 on success, EIO on I2C communication error
 */
static uint8_t nau88c22_set_mic_volume(uint8_t volume)
{
    return nau88c22_set_output_volume(volume, NAU_LEFT_INPUT_PGA_GAIN);
}

/**
 * @brief Set headphone microphone volume
 * @param volume Volume level (0-100)
 * @return 0 on success, EIO on I2C communication error
 */
static uint8_t nau88c22_set_hp_mic_volume(uint8_t volume)
{
    return nau88c22_set_output_volume(volume, NAU_RIGHT_INPUT_PGA_GAIN);
}

/**
 * @brief Set speaker volume
 * @param volume Volume level (0-100)
 * @return 0 on success, EIO on I2C communication error
 */
static uint8_t nau88c22_set_speaker_volume(uint8_t volume)
{
    return nau88c22_set_output_volume_lr(volume, NAU_LSPKOUT_VOLUME, NAU_RSPKOUT_VOLUME);
}

/**
 * @brief Set headphone volume
 * @param volume Volume level (0-100)
 * @return 0 on success, EIO on I2C communication error
 */
static uint8_t nau88c22_set_hp_volume(uint8_t volume)
{
    return nau88c22_set_output_volume_lr(volume, NAU_LHP_VOLUME, NAU_RHP_VOLUME);
}

/**
 * @brief Set equalizer gain for a specific band
 * @param band EQ band (0-4)
 * @param gain Gain value (0-31, 5-bit)
 * @return 0 on success, EINVAL for invalid band, EIO on I2C communication error
 */
static uint8_t nau88c22_set_eq(uint8_t band, uint8_t gain)
{
    if (band > 4)
        return EINVAL;

    uint16_t gain_val = gain & 0x1F; // mask to 5 bits
    uint8_t eq_reg;
    uint8_t err = 0;

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
        return EINVAL;
    }

    uint16_t current_reg_data;
    uint16_t new_reg_data;

    err = nau88c22_read_reg(eq_reg, &current_reg_data);
    if (err != 0)
        return EIO;

    new_reg_data = (current_reg_data & ~0x1F) | gain_val; // update gain bits

    err = nau88c22_write_reg(eq_reg, new_reg_data);
    if (err != 0)
        return EIO;

    return 0;
}

/**
 * @brief Get the NAU88C22 audio driver interface
 * @return Pointer to IAudioDriver_t structure with all driver functions
 */
const IAudioDriver_t *nau88c22_get_driver(void)
{
    static IAudioDriver_t driver = {
        .init = nau88c22_init_void,
        .sleep = nau88c22_sleep_void,
        .speaker = {
            .mute = nau88c22_mute_speaker_void,
            .set_volume = nau88c22_set_speaker_volume_void,
            .mic = {
                .mute = nau88c22_mute_mic_void,
                .set_volume = nau88c22_set_mic_volume_void,
            }},
        .headphones = {
            .mute = nau88c22_mute_hp_void, 
            .set_volume = nau88c22_set_hp_volume_void, 
            .mic = {
                .mute = nau88c22_mute_hp_mic_void,
                .set_volume = nau88c22_set_hp_mic_volume_void,
            }}};
    return &driver;
}
