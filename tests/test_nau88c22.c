#include "nau88c22.h"
#include "bloop_x.h"
#include "sm64_mario_boing.h"
#include "i2c.h"
#include "i2s.h"
#include "gpio.h"

void SystemClock_Config(void);

const int16_t sine_wave[48] = {
    0, 4276, 8480, 12539, 16383, 19947, 23169, 25995,
    28377, 30272, 31650, 32486, 32767, 32486, 31650, 30272,
    28377, 25995, 23169, 19947, 16383, 12539, 8480, 4276,
    0, -4276, -8480, -12539, -16383, -19947, -23169, -25995,
    -28377, -30272, -31650, -32486, -32767, -32486, -31650, -30272,
    -28377, -25995, -23169, -19947, -16383, -12539, -8480, -4276
};

int main(void)
{
    uint8_t status;
    uint16_t i = 0;
    int16_t audio_buffer[2];
    nau88c22_codec_t codec;
    
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_I2S1_Init();

    // Reset the codec first - toggle LED during initialization
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_Delay(100);
    
    // Initialize codec
    status = nau88c22_init(&codec);
    if (status != 0) {
        // Error initializing codec - fast blink
        while(1) {
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
            HAL_Delay(100);
        }
    }

    // Set max volume and unmute
    uint16_t lspk_reg = 0, rspk_reg = 0;
    nau88c22_read_reg(NAU_LSPKOUT_VOLUME, &lspk_reg);
    nau88c22_read_reg(NAU_RSPKOUT_VOLUME, &rspk_reg);
    
    // Clear mute bit (bit 6)
    lspk_reg &= ~(1 << 6);
    // Set max volume (0x3F = 63)
    lspk_reg = (lspk_reg & ~0x3F) | 0x3F;
    
    rspk_reg &= ~(1 << 6);
    rspk_reg = (rspk_reg & ~0x3F) | 0x3F;
    // Set right channel update bit
    rspk_reg |= 0x100;
    
    // Write volume settings
    nau88c22_write_reg(NAU_LSPKOUT_VOLUME, lspk_reg);
    nau88c22_write_reg(NAU_RSPKOUT_VOLUME, rspk_reg);
    
    HAL_Delay(100);
    
    // Verify volume settings
    uint16_t check_reg = 0;
    status = nau88c22_read_reg(NAU_LSPKOUT_VOLUME, &check_reg);
    if ((check_reg & 0x3F) == 0 || (check_reg & (1 << 6))) {
        // Volume is zero or muted - LED pattern (2 blinks)
        for (int i = 0; i < 6; i++) {
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
            HAL_Delay(200);
        }
    }

    // Simple square wave test
    while(1) {
        // audio_buffer[0] = 0;
        // audio_buffer[1] = samples[i];
        

        // uint16_t offset = 0;
        // while (offset < 286303) {
        //     uint16_t chunkSize = (286303 - offset) >= 4096 ? 4096 : (286303 - offset);
        //     HAL_I2S_Transmit(&AUDIO_I2S_HANDLE, (uint16_t*)&samples[offset], chunkSize, HAL_MAX_DELAY);
        //     offset += chunkSize;
        // }

        HAL_I2S_Transmit(&AUDIO_I2S_HANDLE, (uint16_t*)audio, 7840, HAL_MAX_DELAY);
        
        HAL_Delay(4000);
        
        // i = (i + 1) % 143151;
    }
}
