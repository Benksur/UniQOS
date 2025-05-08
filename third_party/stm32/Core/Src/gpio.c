/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins
     PE15   ------> FMC_D12_DA12
     PD7   ------> FMC_NE1
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(VIB_EN_GPIO_Port, VIB_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(AUD_SW_GPIO_Port, AUD_SW_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, MOB_PWR_Pin|MOB_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(UART_DTR_GPIO_Port, UART_DTR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB_HASH_Pin PB_1_Pin PB_2_Pin PB_3_Pin
                           PB_4_Pin PB_STAR_Pin PB_0_Pin */
  GPIO_InitStruct.Pin = PB_HASH_Pin|PB_1_Pin|PB_2_Pin|PB_3_Pin
                          |PB_4_Pin|PB_STAR_Pin|PB_0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PB_5_Pin PB_6_Pin PB_7_Pin PB_MENU_L_Pin
                           PB_MENU_R_Pin PB_VOL_UP_Pin PB_CALL_Pin */
  GPIO_InitStruct.Pin = PB_5_Pin|PB_6_Pin|PB_7_Pin|PB_MENU_L_Pin
                          |PB_MENU_R_Pin|PB_VOL_UP_Pin|PB_CALL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB_8_Pin PB_9_Pin */
  GPIO_InitStruct.Pin = PB_8_Pin|PB_9_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 PA2 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : VIB_EN_Pin */
  GPIO_InitStruct.Pin = VIB_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(VIB_EN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : HP_DET_Pin UART_RI_Pin SDMMC_DET_Pin */
  GPIO_InitStruct.Pin = HP_DET_Pin|UART_RI_Pin|SDMMC_DET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : AUD_SW_Pin MOB_PWR_Pin MOB_RST_Pin */
  GPIO_InitStruct.Pin = AUD_SW_Pin|MOB_PWR_Pin|MOB_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : MOB_WAKE_Pin FUEL_GPOUT_Pin MCP_PG_Pin MCP_STAT2_Pin
                           MCP_STAT1_Pin */
  GPIO_InitStruct.Pin = MOB_WAKE_Pin|FUEL_GPOUT_Pin|MCP_PG_Pin|MCP_STAT2_Pin
                          |MCP_STAT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PE7 PE8 PE9 PE10
                           PE11 PE12 PE13 PE14 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : DISP_D12_Pin */
  GPIO_InitStruct.Pin = DISP_D12_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(DISP_D12_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PD8 PD9 PD10 PD11
                           PD14 PD15 PD0 PD1 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : UART_DCD_Pin */
  GPIO_InitStruct.Pin = UART_DCD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(UART_DCD_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : UART_DTR_Pin */
  GPIO_InitStruct.Pin = UART_DTR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(UART_DTR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : UART_DSR_Pin */
  GPIO_InitStruct.Pin = UART_DSR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(UART_DSR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PB_HANG_Pin */
  GPIO_InitStruct.Pin = PB_HANG_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(PB_HANG_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : DISP_CS_Pin */
  GPIO_InitStruct.Pin = DISP_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(DISP_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PB_DPAD_UP_Pin PB_DPAD_DOWN_Pin PB_DPAD_RIGHT_Pin PB_DPAD_LEFT_Pin
                           PB_DPAD_SELECT_Pin */
  GPIO_InitStruct.Pin = PB_DPAD_UP_Pin|PB_DPAD_DOWN_Pin|PB_DPAD_RIGHT_Pin|PB_DPAD_LEFT_Pin
                          |PB_DPAD_SELECT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
