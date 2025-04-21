#include "nau88c22.h"
#include "stm32_config.h"

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
    
    board_init();
    
    status = nau88c22_init();
    if (!status) {
        while(1) {
            HAL_GPIO_TogglePin(ERROR_LED_GPIO_Port, ERROR_LED_Pin);
            HAL_Delay(100);
        }
    }
    
    nau88c22_set_volume(80);
    
    while(1) {
        audio_buffer[0] = sine_wave[i];
        audio_buffer[1] = sine_wave[i];
        
        HAL_I2S_Transmit(&AUDIO_I2S_HANDLE, (uint16_t*)audio_buffer, 2, HAL_MAX_DELAY);
        
        i = (i + 1) % 48;
    }
}