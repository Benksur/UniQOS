#ifndef STM32_CONFIG_H
#define STM32_CONFIG_H

#include "stm32h7xx_hal.h"

#define AUDIO_I2C_HANDLE      hi2c1
#define AUDIO_I2S_HANDLE      hi2s2
#define DISPLAY_SPI_HANDLE    hspi1
#define MODEM_UART_HANDLE     huart2

extern I2C_HandleTypeDef AUDIO_I2C_HANDLE;
extern I2S_HandleTypeDef AUDIO_I2S_HANDLE;
extern SPI_HandleTypeDef DISPLAY_SPI_HANDLE;
extern UART_HandleTypeDef MODEM_UART_HANDLE;

#endif