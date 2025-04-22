#ifndef DRV2603_H
#define DRV2603_H

#include "board/stm32_config.h"

void drv2603_init(TIM_HandleTypeDef *htim_pwm);
void drv2603_enable(uint8_t enable);
void drv2603_set_strength(uint8_t strength_percent);

#endif
