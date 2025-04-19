#ifndef NAU88C22_H
#define NAU88C22_H

#include <stdint.h>

#define NAU88C22_I2C_ADDR 0x1A

/* Register addresses from datasheet */
#define NAU_RESET 0x00
#define NAU_PWR1 0x01
#define NAU_PWR2 0x02
#define NAU_PWR3 0x03
#define NAU_AINTF 0x04
#define NAU_COMPANDING 0x05
#define NAU_CLOCK1 0x06
#define NAU_CLOCK2 0x07
#define NAU_GPIO 0x08
#define NAU_JACK_DETECT1 0x09
#define NAU_DAC_CTRL 0x0A
#define NAU_LEFT_DAC_VOL 0x0B
#define NAU_RIGHT_DAC_VOL 0x0C
#define NAU_JACK_DETECT2 0x0D
#define NAU_ADC_CTRL 0x0E
#define NAU_LEFT_ADC_VOL 0x0F
#define NAU_RIGHT_ADC_VOL 0x10
#define NAU_EQ1 0x12
#define NAU_EQ2 0x13
#define NAU_EQ3 0x14
#define NAU_EQ4 0x15
#define NAU_EQ5 0x16
#define NAU_DAC_LIMITER1 0x18
#define NAU_DAC_LIMITER2 0x19
#define NAU_NOTCH_FILTER1 0x1B
#define NAU_NOTCH_FILTER2 0x1C
#define NAU_NOTCH_FILTER3 0x1D
#define NAU_NOTCH_FILTER4 0x1E
#define NAU_ALC_CTRL1 0x20
#define NAU_ALC_CTRL2 0x21
#define NAU_ALC_CTRL3 0x22
#define NAU_NOISE_GATE 0x23
#define NAU_PPL_N 0x24
#define NAU_PPL_K1 0x25
#define NAU_PPL_K2 0x26
#define NAU_PPL_K3 0x27
#define NAU_3D_CONTROL 0x29
#define NAU_RIGHT_SPEAKER_SUBMIXER 0x2B
#define NAU_INPUT_CONTROL 0x2C
#define NAU_LEFT_INPUT_PGA_GAIN 0x2D
#define NAU_RIGHT_INPUT_PGA_GAIN 0x2E
#define NAU_LEFT_ADC_BOOST 0x2F
#define NAU_RIGHT_ADC_BOOST 0x30
#define NAU_OUTPUT_CONTROL 0x31
#define NAU_LEFT_MIXER 0x32
#define NAU_RIGHT_MIXER 0x33
#define NAU_LHP_VOLUME 0x34
#define NAU_RHP_VOLUME 0x35
#define NAU_LSPKOUT_VOLUME 0x36
#define NAU_RSPKOUT_VOLUME 0x37
#define NAU_AUX2_MIXER 0x38
#define NAU_AUX1_MIXER 0x39
#define NAU_PWR4 0x3A
#define NAU_LEFT_TIME_SLOT 0x3B
#define NAU_MISC 0x3C
#define NAU_RIGHT_TIME_SLOT 0x3D
#define NAU_REVISION 0x3E
#define NAU_DEVICE_ID 0x3F

uint8_t nau88c22_init(void);
uint8_t nau88c22_hp_mic_toggle(uint8_t enable);
uint8_t nau88c22_hp_detect(void);
uint8_t nau88c22_write_reg(uint8_t reg_addr, uint16_t reg_data);
uint8_t nau88c22_read_reg(uint8_t reg_addr, uint16_t *reg_data);
uint8_t nau88c22_set_output_volume(uint8_t volume, uint8_t left_reg, uint8_t right_reg);
uint8_t nau88c22_increment_output_volume(uint8_t increment);
uint8_t nau88c22_mute_output(uint8_t enable);
uint8_t nau88c22_set_eq(uint8_t band, uint8_t gain);


#endif