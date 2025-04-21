#ifndef DRV2603_H
#define DRV2603_H

#include "stm32h7xx_hal.h"
#include "board/stm32_config.h"

#define DRV2603_ENABLE_PORT GPIOC
#define DRV2603_ENABLE_PIN GPIO_PIN_4
#define DRV2603_PWM_PORT GPIOA
#define DRV2603_PWM_PIN GPIO_PIN_6

void drv2603_init(TIM_HandleTypeDef *htim_pwm);
void drv2603_enable(uint8_t enable);
void drv2603_set_strength(uint8_t strength_percent);

#endif
