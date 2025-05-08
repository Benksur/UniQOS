/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void SystemClock_Config(void);

/* Peripheral handles */
extern I2C_HandleTypeDef hi2c1;
extern I2S_HandleTypeDef hi2s1;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_tim5_ch4;
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PB_HASH_Pin GPIO_PIN_2
#define PB_HASH_GPIO_Port GPIOE
#define PB_1_Pin GPIO_PIN_3
#define PB_1_GPIO_Port GPIOE
#define PB_2_Pin GPIO_PIN_4
#define PB_2_GPIO_Port GPIOE
#define PB_3_Pin GPIO_PIN_5
#define PB_3_GPIO_Port GPIOE
#define PB_4_Pin GPIO_PIN_6
#define PB_4_GPIO_Port GPIOE
#define PB_5_Pin GPIO_PIN_13
#define PB_5_GPIO_Port GPIOC
#define PB_6_Pin GPIO_PIN_14
#define PB_6_GPIO_Port GPIOC
#define PB_7_Pin GPIO_PIN_15
#define PB_7_GPIO_Port GPIOC
#define PB_8_Pin GPIO_PIN_0
#define PB_8_GPIO_Port GPIOH
#define PB_9_Pin GPIO_PIN_1
#define PB_9_GPIO_Port GPIOH
#define PB_MENU_L_Pin GPIO_PIN_0
#define PB_MENU_L_GPIO_Port GPIOC
#define PB_MENU_R_Pin GPIO_PIN_1
#define PB_MENU_R_GPIO_Port GPIOC
#define PB_VOL_UP_Pin GPIO_PIN_2
#define PB_VOL_UP_GPIO_Port GPIOC
#define PB_CALL_Pin GPIO_PIN_3
#define PB_CALL_GPIO_Port GPIOC
#define RGB_DATA_Pin GPIO_PIN_3
#define RGB_DATA_GPIO_Port GPIOA
#define VIB_PWM_Pin GPIO_PIN_6
#define VIB_PWM_GPIO_Port GPIOA
#define VIB_EN_Pin GPIO_PIN_4
#define VIB_EN_GPIO_Port GPIOC
#define HP_DET_Pin GPIO_PIN_5
#define HP_DET_GPIO_Port GPIOC
#define AUD_SW_Pin GPIO_PIN_0
#define AUD_SW_GPIO_Port GPIOB
#define MOB_WAKE_Pin GPIO_PIN_1
#define MOB_WAKE_GPIO_Port GPIOB
#define MOB_PWR_Pin GPIO_PIN_2
#define MOB_PWR_GPIO_Port GPIOB
#define DISP_D12_Pin GPIO_PIN_15
#define DISP_D12_GPIO_Port GPIOE
#define DISP_BL_Pin GPIO_PIN_10
#define DISP_BL_GPIO_Port GPIOB
#define MOB_RST_Pin GPIO_PIN_11
#define MOB_RST_GPIO_Port GPIOB
#define FUEL_GPOUT_Pin GPIO_PIN_12
#define FUEL_GPOUT_GPIO_Port GPIOB
#define MCP_PG_Pin GPIO_PIN_13
#define MCP_PG_GPIO_Port GPIOB
#define MCP_STAT2_Pin GPIO_PIN_14
#define MCP_STAT2_GPIO_Port GPIOB
#define MCP_STAT1_Pin GPIO_PIN_15
#define MCP_STAT1_GPIO_Port GPIOB
#define UART_DCD_Pin GPIO_PIN_12
#define UART_DCD_GPIO_Port GPIOD
#define UART_DTR_Pin GPIO_PIN_13
#define UART_DTR_GPIO_Port GPIOD
#define UART_RI_Pin GPIO_PIN_6
#define UART_RI_GPIO_Port GPIOC
#define SDMMC_DET_Pin GPIO_PIN_7
#define SDMMC_DET_GPIO_Port GPIOC
#define UART_DSR_Pin GPIO_PIN_8
#define UART_DSR_GPIO_Port GPIOA
#define PB_HANG_Pin GPIO_PIN_15
#define PB_HANG_GPIO_Port GPIOA
#define DISP_CS_Pin GPIO_PIN_7
#define DISP_CS_GPIO_Port GPIOD
#define PB_DPAD_UP_Pin GPIO_PIN_3
#define PB_DPAD_UP_GPIO_Port GPIOB
#define PB_DPAD_DOWN_Pin GPIO_PIN_4
#define PB_DPAD_DOWN_GPIO_Port GPIOB
#define PB_DPAD_RIGHT_Pin GPIO_PIN_5
#define PB_DPAD_RIGHT_GPIO_Port GPIOB
#define PB_DPAD_LEFT_Pin GPIO_PIN_8
#define PB_DPAD_LEFT_GPIO_Port GPIOB
#define PB_DPAD_SELECT_Pin GPIO_PIN_9
#define PB_DPAD_SELECT_GPIO_Port GPIOB
#define PB_STAR_Pin GPIO_PIN_0
#define PB_STAR_GPIO_Port GPIOE
#define PB_0_Pin GPIO_PIN_1
#define PB_0_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
