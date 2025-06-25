#include "modem.h"
#include "stm32_config.h"

int main(void)
{
    uint8_t status;
    
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    
    status = rc7620_init();
    
    while(1) {}
}
