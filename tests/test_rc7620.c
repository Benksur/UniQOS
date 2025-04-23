#include "rc7620.h"
#include "stm32_config.h"

int main(void)
{
    uint8_t status;

    board_init();
    
    status = rc7620_init();
    
    while(1) {
    }
}