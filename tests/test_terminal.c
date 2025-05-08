#include "modem_terminal.h"
#include "gpio.h"
#include "usart.h"

#define INPUT_BUFFER_SIZE 128
#define RESPONSE_BUFFER_SIZE 1024

void main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_USART3_UART_Init();
    
    uint8_t rx_buffer[RESPONSE_BUFFER_SIZE];
    HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CMGF=1\n", 10, HAL_MAX_DELAY);
    while (1) {
        memset(rx_buffer, 0, sizeof(rx_buffer));
        HAL_UART_Transmit(&huart2, (uint8_t *)"AT+CMGR=\"REC READ\"\n", 20, HAL_MAX_DELAY);
        HAL_Delay(10000);
        
        HAL_UART_Receive(&MODEM_UART_HANDLE, rx_buffer, RESPONSE_BUFFER_SIZE, 1000);
        // HAL_Delay(10000);
        DEBUG_PRINTF("Received: %s\r\n", rx_buffer);
    }
    

    // modem_terminal_test();
}
