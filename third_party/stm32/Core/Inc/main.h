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

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MOB_RST_Pin GPIO_PIN_14
#define MOB_RST_GPIO_Port GPIOC
#define MOB_PWR_Pin GPIO_PIN_13
#define MOB_PWR_GPIO_Port GPIOC
#define PB_0_Pin GPIO_PIN_2
#define PB_0_GPIO_Port GPIOE
#define PB_2_Pin GPIO_PIN_7
#define PB_2_GPIO_Port GPIOB
#define PB_3_Pin GPIO_PIN_4
#define PB_3_GPIO_Port GPIOB
#define PB_CALL_Pin GPIO_PIN_15
#define PB_CALL_GPIO_Port GPIOC
#define PB_DPAD_DOWN_Pin GPIO_PIN_3
#define PB_DPAD_DOWN_GPIO_Port GPIOE
#define PB_9_Pin GPIO_PIN_6
#define PB_9_GPIO_Port GPIOB
#define PB_6_Pin GPIO_PIN_5
#define PB_6_GPIO_Port GPIOD
#define PB_4_Pin GPIO_PIN_0
#define PB_4_GPIO_Port GPIOH
#define PB_1_Pin GPIO_PIN_4
#define PB_1_GPIO_Port GPIOE
#define PB_8_Pin GPIO_PIN_1
#define PB_8_GPIO_Port GPIOE
#define PB_5_Pin GPIO_PIN_5
#define PB_5_GPIO_Port GPIOB
#define PB_DPAD_RIGHT_Pin GPIO_PIN_6
#define PB_DPAD_RIGHT_GPIO_Port GPIOD
#define UART_DCD_Pin GPIO_PIN_3
#define UART_DCD_GPIO_Port GPIOD
#define PB_7_Pin GPIO_PIN_5
#define PB_7_GPIO_Port GPIOE
#define PB_HASH_Pin GPIO_PIN_0
#define PB_HASH_GPIO_Port GPIOE
#define PB_END_CALL_Pin GPIO_PIN_7
#define PB_END_CALL_GPIO_Port GPIOD
#define PB_MENU_R_Pin GPIO_PIN_4
#define PB_MENU_R_GPIO_Port GPIOD
#define UART_DTR_Pin GPIO_PIN_0
#define UART_DTR_GPIO_Port GPIOD
#define UART_DSR_Pin GPIO_PIN_8
#define UART_DSR_GPIO_Port GPIOA
#define PB_STAR_Pin GPIO_PIN_6
#define PB_STAR_GPIO_Port GPIOE
#define SDMMC_DET_Pin GPIO_PIN_7
#define SDMMC_DET_GPIO_Port GPIOC
#define PB_DPAD_SELECT_Pin GPIO_PIN_0
#define PB_DPAD_SELECT_GPIO_Port GPIOC
#define PB_DPAD_UP_Pin GPIO_PIN_1
#define PB_DPAD_UP_GPIO_Port GPIOC
#define UART_RI_Pin GPIO_PIN_6
#define UART_RI_GPIO_Port GPIOC
#define PB_MENU_L_Pin GPIO_PIN_0
#define PB_MENU_L_GPIO_Port GPIOA
#define PB_DPAD_LEFT_Pin GPIO_PIN_4
#define PB_DPAD_LEFT_GPIO_Port GPIOC
#define DISP_RES_Pin GPIO_PIN_15
#define DISP_RES_GPIO_Port GPIOD
#define FUEL_GPO_Pin GPIO_PIN_11
#define FUEL_GPO_GPIO_Port GPIOD
#define MCP_PG_Pin GPIO_PIN_15
#define MCP_PG_GPIO_Port GPIOB
#define PB_VOL_DOWN_Pin GPIO_PIN_1
#define PB_VOL_DOWN_GPIO_Port GPIOA
#define DISP_DC_Pin GPIO_PIN_14
#define DISP_DC_GPIO_Port GPIOD
#define MCP_STAT2_Pin GPIO_PIN_10
#define MCP_STAT2_GPIO_Port GPIOD
#define MCP_STAT1_Pin GPIO_PIN_14
#define MCP_STAT1_GPIO_Port GPIOB
#define PB_VOL_UP_Pin GPIO_PIN_2
#define PB_VOL_UP_GPIO_Port GPIOA
#define DISP_BL_Pin GPIO_PIN_6
#define DISP_BL_GPIO_Port GPIOA
#define DISP_CS_Pin GPIO_PIN_13
#define DISP_CS_GPIO_Port GPIOB
#define MOB_PWR_REM_Pin GPIO_PIN_9
#define MOB_PWR_REM_GPIO_Port GPIOD
#define HP_DET_Pin GPIO_PIN_13
#define HP_DET_GPIO_Port GPIOD
#define RGB_DATA_Pin GPIO_PIN_3
#define RGB_DATA_GPIO_Port GPIOA
#define VIB_PWM_Pin GPIO_PIN_1
#define VIB_PWM_GPIO_Port GPIOB
#define VIB_EN_Pin GPIO_PIN_9
#define VIB_EN_GPIO_Port GPIOE
#define PB_PWR_Pin GPIO_PIN_12
#define PB_PWR_GPIO_Port GPIOB
#define MOB_WAKE_Pin GPIO_PIN_8
#define MOB_WAKE_GPIO_Port GPIOD
#define AUDIO_SW_Pin GPIO_PIN_12
#define AUDIO_SW_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
