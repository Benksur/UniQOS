/**
 * @file drv2603.h
 * @brief DRV2603 haptic motor driver
 * @ingroup haptic_driver
 *
 * Driver for the Texas Instruments DRV2603 haptic motor controller.
 * Provides vibration control for both LRA and ERM motor types.
 */

#ifndef DRV2603_H
#define DRV2603_H

#include "stm32_config.h"
#include "tim.h"

/** @ingroup haptic_driver
 *  @brief DRV2603 prefix for function names */
#define drv2603_

/** @ingroup haptic_driver
 *  @brief Timer handle for DRV2603 PWM control */
#define DRV2603_TIM &htim3

/** @ingroup haptic_driver
 *  @brief Timer channel for DRV2603 control */
#define DRV2603_TIM_CHANEL TIM_CHANNEL_4

/**
 * @ingroup haptic_driver
 * @brief Initialize the DRV2603 haptic driver
 * @return 0 on success, error code otherwise
 */
uint8_t drv2603_init();

/**
 * @ingroup haptic_driver
 * @brief Enable or disable haptic motor
 * @param enable 1 to enable, 0 to disable
 */
void drv2603_enable(uint8_t enable);

/**
 * @ingroup haptic_driver
 * @brief Set vibration strength for LRA motor
 * @param strength_percent Vibration strength (0-100%)
 * @return 0 on success, error code otherwise
 */
uint8_t drv2603_set_strength_lra(uint8_t strength_percent);

/**
 * @ingroup haptic_driver
 * @brief Set vibration strength for ERM motor
 * @param strength_percent Vibration strength (0-100%)
 * @return 0 on success, error code otherwise
 */
uint8_t drv2603_set_strength_erm(uint8_t strength_percent);

#endif
