#ifndef DRV2603_H
#define DRV2603_H

#include "stm32_config.h"
#include "tim.h"

#define drv2603_

#define DRV2603_TIM &htim3
#define DRV2603_TIM_CHANEL TIM_CHANNEL_4

uint8_t drv2603_init();
void drv2603_enable(uint8_t enable);
uint8_t drv2603_set_strength_lra(uint8_t strength_percent);
uint8_t drv2603_set_strength_erm(uint8_t strength_percent);

#endif
