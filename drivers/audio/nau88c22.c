#include "nau88c22.h"
#include "errornum.h"

// Static function prototypes
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
static nau_mute_states_t nau88c22_save_and_mute_all(void);
static void nau88c22_restore_mute_state(nau_mute_states_t saved);
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

static uint8_t nau88c22_write_reg(uint8_t reg_addr, uint16_t reg_data)
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

static uint8_t nau88c22_read_reg(uint8_t reg_addr, uint16_t *reg_data)
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
        {NAU_PPL_N, 0x019},  // N of 9
        // fractional portion is 0x6872B0
        {NAU_PPL_K1, 0x1A},  //highest 6 bits of fractional portion
        {NAU_PPL_K2, 0x039}, // middle 9 bits of fractional portion
        {NAU_PPL_K3, 0x0B0}, // lowest 9 bits of fractional portion
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

    return 0;
}

static uint8_t nau88c22_enable_lin_mic(void) {
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

// General-purpose mute helper for a single register
static uint8_t nau88c22_mute_reg(uint8_t reg_addr, uint8_t enable, uint8_t add_0x100)
{
    uint16_t reg_data;
    const uint16_t mute_mask = (1 << 6);
    uint8_t err = nau88c22_read_reg(reg_addr, &reg_data);
    if (err != 0) return EIO;
    if (enable) reg_data |= mute_mask; else reg_data &= ~mute_mask;
    if (add_0x100) reg_data |= 0x100;
    err = nau88c22_write_reg(reg_addr, reg_data);
    if (err != 0) return EIO;
    return 0;
}

// General-purpose mute helper for left/right register pair
static uint8_t nau88c22_mute_lr(uint8_t left_reg, uint8_t right_reg, uint8_t enable)
{
    uint8_t err;
    err = nau88c22_mute_reg(left_reg, enable, 0);
    if (err != 0) return err;
    err = nau88c22_mute_reg(right_reg, enable, 1);
    if (err != 0) return err;
    return 0;
}

static uint8_t nau88c22_mute_speaker(uint8_t enable)
{
    return nau88c22_mute_lr(NAU_LSPKOUT_VOLUME, NAU_RSPKOUT_VOLUME, enable);
}

static uint8_t nau88c22_mute_hp(uint8_t enable)
{
    return nau88c22_mute_lr(NAU_LHP_VOLUME, NAU_RHP_VOLUME, enable);
}

static uint8_t nau88c22_mute_mic(uint8_t enable)
{
    return nau88c22_mute_reg(NAU_LEFT_INPUT_PGA_GAIN, enable, 0);
}

static uint8_t nau88c22_mute_hp_mic(uint8_t enable)
{
    return nau88c22_mute_reg(NAU_RIGHT_INPUT_PGA_GAIN, enable, 0);
}

static void nau88c22_mute_all(uint8_t enable)
{
    nau88c22_mute_speaker(enable);
    nau88c22_mute_hp(enable);
    nau88c22_mute_mic(enable);
    nau88c22_mute_hp_mic(enable);
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

static nau_mute_states_t nau88c22_save_and_mute_all(void)
{
    nau_mute_states_t saved;
    saved.speaker = nau88c22_is_muted(NAU_LHP_VOLUME);
    saved.mic = nau88c22_is_muted(NAU_LEFT_INPUT_PGA_GAIN);
    saved.hp_mic = nau88c22_is_muted(NAU_RIGHT_INPUT_PGA_GAIN);
    saved.hp_out = nau88c22_is_muted(NAU_LHP_VOLUME);

    nau88c22_mute_all(1);
    return saved;
}

static void nau88c22_restore_mute_state(nau_mute_states_t saved)
{
    nau88c22_mute_speaker(saved.speaker);
    nau88c22_mute_hp(saved.hp_out);
    nau88c22_mute_mic(saved.mic);
    nau88c22_mute_hp_mic(saved.hp_mic);
}

static uint8_t nau88c22_sleep(uint8_t enable)
{

    uint16_t reg_data;
    uint8_t err = 0;

    nau_mute_states_t saved = nau88c22_save_and_mute_all();
    nau88c22_read_reg(NAU_PWR2, &reg_data);

    if (enable)
        reg_data |= 0x040;
    else
        reg_data &= ~0x040;

    err = nau88c22_write_reg(NAU_PWR2, reg_data);
    if (err != 0)
    {
        nau88c22_restore_mute_state(saved);
        return EIO;
    }

    nau88c22_restore_mute_state(saved);
    return 0;
}

static uint8_t nau88c22_hp_mic_toggle(uint8_t enable)
{

    uint8_t err = 0;
    uint16_t reg_value = enable ? 0x02A : 0x015;

    nau_mute_states_t saved = nau88c22_save_and_mute_all();
    err = nau88c22_write_reg(NAU_PWR2, reg_value);
    if (err != 0)
    {
        nau88c22_restore_mute_state(saved);
        return EIO;
    }

    nau88c22_restore_mute_state(saved);
    return 0;
}

static uint8_t nau88c22_hp_detect(void)
{
    return HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5);
}

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

// General-purpose set volume helper for a single register
static uint8_t nau88c22_set_volume_reg(uint8_t reg_addr, uint8_t volume, uint8_t add_0x100)
{
    uint16_t reg_data;
    uint8_t err = nau88c22_read_reg(reg_addr, &reg_data);
    if (err != 0) return EIO;
    uint16_t vol_val;
    if (volume == 0) {
        vol_val = 0x000;
    } else {
        if (volume > 100) volume = 100;
        vol_val = (volume * 63) / 100;
        vol_val &= 0x3F;
    }
    reg_data = (reg_data & ~0x3F) | vol_val;
    if (add_0x100) reg_data |= 0x100;
    err = nau88c22_write_reg(reg_addr, reg_data);
    if (err != 0) return EIO;
    return 0;
}

// General-purpose set volume helper for left/right register pair
static uint8_t nau88c22_set_volume_lr(uint8_t left_reg, uint8_t right_reg, uint8_t volume)
{
    uint8_t err;
    err = nau88c22_set_volume_reg(left_reg, volume, 0);
    if (err != 0) return err;
    err = nau88c22_set_volume_reg(right_reg, volume, 1);
    if (err != 0) return err;
    return 0;
}

static uint8_t nau88c22_set_output_volume(uint8_t volume, uint8_t reg_addr)
{
    uint8_t err = 0;
    nau_mute_states_t saved = nau88c22_save_and_mute_all();

    if (err != 0)
        return err;

    err = nau88c22_set_volume_reg(reg_addr, volume, 0);
    if (err != 0) {
        nau88c22_restore_mute_state(saved);
        return EIO;
    };

    nau88c22_restore_mute_state(saved);
    return 0;
}

static uint8_t nau88c22_set_output_volume_lr(uint8_t volume, uint8_t left_reg, uint8_t right_reg)
{
    uint8_t err = 0;
    nau_mute_states_t saved = nau88c22_save_and_mute_all();

    err = nau88c22_set_volume_lr(left_reg, right_reg, volume);
    if (err != 0) {
        nau88c22_restore_mute_state(saved);
        return EIO;
    };

    nau88c22_restore_mute_state(saved);
    return 0;
}



static uint8_t nau88c22_set_mic_volume(uint8_t volume)
{
    return nau88c22_set_output_volume(volume, NAU_LEFT_INPUT_PGA_GAIN);
}

static uint8_t nau88c22_set_hp_mic_volume(uint8_t volume)
{
    return nau88c22_set_output_volume(volume, NAU_RIGHT_INPUT_PGA_GAIN);
}

static uint8_t nau88c22_set_speaker_volume(uint8_t volume)
{
    return nau88c22_set_output_volume_lr(volume, NAU_LSPKOUT_VOLUME, NAU_RSPKOUT_VOLUME);
}

static uint8_t nau88c22_set_hp_volume(uint8_t volume)
{
    return nau88c22_set_output_volume_lr(volume, NAU_LHP_VOLUME, NAU_RHP_VOLUME);
}



static uint8_t nau88c22_set_eq(uint8_t band, uint8_t gain)
{
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

    nau_mute_states_t saved = nau88c22_save_and_mute_all();
    if (err != 0)
        return err;

    uint16_t current_reg_data;
    uint16_t new_reg_data;

    err = nau88c22_read_reg(eq_reg, &current_reg_data);
    if (err != 0) goto set_eq_cleanup_err;

    new_reg_data = (current_reg_data & ~0x1F) | gain_val;

    err = nau88c22_write_reg(eq_reg, new_reg_data);
    if (err != 0) goto set_eq_cleanup_err;

    nau88c22_restore_mute_state(saved);
    return 0;

set_eq_cleanup_err:
    nau88c22_restore_mute_state(saved);
    return EIO;
}

const IAudioDriver_t* nau88c22_get_driver(void) {
    static IAudioDriver_t driver = {
        .init = nau88c22_init_void,
        .sleep = nau88c22_sleep_void,
        .speaker = {
            .mute = nau88c22_mute_speaker_void,
            .set_volume = nau88c22_set_speaker_volume_void,
            .mic = {
                .mute = nau88c22_mute_mic_void,
                .set_volume = nau88c22_set_mic_volume_void,
            }
        },
        .headphones = {
            .mute = nau88c22_mute_hp_void,
            .set_volume = nau88c22_set_hp_volume_void,
            .mic = {
                .mute = nau88c22_mute_hp_mic_void,
                .set_volume = nau88c22_set_hp_mic_volume_void,
            }
        }
    };
    return &driver;
}
