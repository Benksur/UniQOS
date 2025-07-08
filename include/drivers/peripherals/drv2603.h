#ifndef DRV2603_H
#define DRV2603_H

#include "stm32_config.h"

uint8_t drv2603_init(TIM_HandleTypeDef *htim_pwm);
void drv2603_enable(uint8_t enable);
uint8_t drv2603_set_strength_lra(uint8_t strength_percent);
uint8_t drv2603_set_strength_erm(uint8_t strength_percent);

#endif
