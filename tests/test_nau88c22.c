#include "nau88c22.h"
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

    status = nau88c22_enable_jlin(&codec);
    if (status != 0) {
        // Error enabling JLIN - fast blink
        while(1) {
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
            HAL_Delay(100);
        }
    }

    nau88c22_set_output_volume(&codec, 100, NAU_LSPKOUT_VOLUME, NAU_RSPKOUT_VOLUME);
    nau88c22_set_output_volume(&codec, 0, NAU_LHP_VOLUME, NAU_RHP_VOLUME);
    
    while(1) {

        HAL_I2S_Transmit(&AUDIO_I2S_HANDLE, (uint16_t*)audio, 7840, HAL_MAX_DELAY);
        if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5)) {
            for (uint8_t x = 0; i < 10; i++){
                HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
                HAL_Delay(20);
            }
            
        }
        
        HAL_Delay(2000);
        
        // i = (i + 1) % 143151;
    }
}
