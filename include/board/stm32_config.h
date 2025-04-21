#ifndef STM32_CONFIG_H
#define STM32_CONFIG_H

#include "stm32h7xx_hal.h"

#ifdef DEBUG
#include <stdio.h>
#define DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif

#define AUDIO_I2C_HANDLE hi2c1
#define AUDIO_I2S_HANDLE hi2s1
#define DISPLAY_SPI_HANDLE hspi1
#define MODEM_UART_HANDLE huart1
#define DEBUG_UART_HANDLE huart3

#define MCP73871_STAT1_PORT GPIOB
#define MCP73871_STAT1_PIN GPIO_PIN_15

#define MCP73871_STAT2_PORT GPIOB
#define MCP73871_STAT2_PIN GPIO_PIN_14

#define MCP73871_PG_PORT GPIOB
#define MCP73871_PG_PIN GPIO_PIN_13

#define MODEM_POWER_PORT GPIOC
#define MODEM_POWER_PIN GPIO_PIN_6

#define DRV2603_ENABLE_PORT GPIOC
#define DRV2603_ENABLE_PIN GPIO_PIN_4
#define DRV2603_PWM_PORT GPIOA
#define DRV2603_PWM_PIN GPIO_PIN_6

extern I2C_HandleTypeDef hi2c1;
extern I2S_HandleTypeDef hi2s1;
extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

#endif