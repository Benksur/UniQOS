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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void stm32_board_init(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
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
#define DISP_D4_Pin GPIO_PIN_7
#define DISP_D4_GPIO_Port GPIOE
#define DISP_D5_Pin GPIO_PIN_8
#define DISP_D5_GPIO_Port GPIOE
#define DISP_D6_Pin GPIO_PIN_9
#define DISP_D6_GPIO_Port GPIOE
#define DISP_D7_Pin GPIO_PIN_10
#define DISP_D7_GPIO_Port GPIOE
#define DISP_D8_Pin GPIO_PIN_11
#define DISP_D8_GPIO_Port GPIOE
#define DISP_D9_Pin GPIO_PIN_12
#define DISP_D9_GPIO_Port GPIOE
#define DISP_D10_Pin GPIO_PIN_13
#define DISP_D10_GPIO_Port GPIOE
#define DISP_D11_Pin GPIO_PIN_14
#define DISP_D11_GPIO_Port GPIOE
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
#define DISP_D13_Pin GPIO_PIN_8
#define DISP_D13_GPIO_Port GPIOD
#define DISP_D14_Pin GPIO_PIN_9
#define DISP_D14_GPIO_Port GPIOD
#define DISP_D15_Pin GPIO_PIN_10
#define DISP_D15_GPIO_Port GPIOD
#define DISP_DC_Pin GPIO_PIN_11
#define DISP_DC_GPIO_Port GPIOD
#define UART_DCD_Pin GPIO_PIN_12
#define UART_DCD_GPIO_Port GPIOD
#define UART_DTR_Pin GPIO_PIN_13
#define UART_DTR_GPIO_Port GPIOD
#define DISP_D0_Pin GPIO_PIN_14
#define DISP_D0_GPIO_Port GPIOD
#define DISP_D1_Pin GPIO_PIN_15
#define DISP_D1_GPIO_Port GPIOD
#define UART_RI_Pin GPIO_PIN_6
#define UART_RI_GPIO_Port GPIOC
#define SDMMC_DET_Pin GPIO_PIN_7
#define SDMMC_DET_GPIO_Port GPIOC
#define UART_DSR_Pin GPIO_PIN_8
#define UART_DSR_GPIO_Port GPIOA
#define DISP_D2_Pin GPIO_PIN_0
#define DISP_D2_GPIO_Port GPIOD
#define DISP_D3_Pin GPIO_PIN_1
#define DISP_D3_GPIO_Port GPIOD
#define DISP_RES_Pin GPIO_PIN_3
#define DISP_RES_GPIO_Port GPIOD
#define DISP_RD_Pin GPIO_PIN_4
#define DISP_RD_GPIO_Port GPIOD
#define DISP_WR_Pin GPIO_PIN_5
#define DISP_WR_GPIO_Port GPIOD
#define DISP_CS_Pin GPIO_PIN_7
#define DISP_CS_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
