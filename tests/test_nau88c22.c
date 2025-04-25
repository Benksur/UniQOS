#include "nau88c22.h"
#include "stm32_config.h"
#include "gpio.h"

void SystemClock_Config(void);
void Error_Handler(void);

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
    uint16_t audio_buffer[2];
    nau88c22_codec_t codec;
    
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    
    status = nau88c22_init(&codec);
    if (status != 0) {
        while(1) {
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
            HAL_Delay(100);
        }
    }
    
    status = nau88c22_set_output_volume(&codec, 80, NAU_LHP_VOLUME, NAU_RHP_VOLUME);
    if (status != 0) {
        while(1) {
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
            HAL_Delay(100);
        }
    }
    
    while(1) {
        audio_buffer[0] = sine_wave[i];
        audio_buffer[1] = sine_wave[i];
        
        HAL_I2S_Transmit(&AUDIO_I2S_HANDLE, (uint16_t*)audio_buffer, 2, HAL_MAX_DELAY);
        
        i = (i + 1) % 48;
    }
}