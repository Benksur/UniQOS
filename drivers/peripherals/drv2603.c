#include "drv2603.h"
#include "errornum.h"

static TIM_HandleTypeDef *htim_drv_pwm = NULL;

uint8_t drv2603_init(TIM_HandleTypeDef *htim_pwm)
{
    if (htim_pwm == NULL)
    {
        return EINVAL;
    }
    
    htim_drv_pwm = htim_pwm;
    HAL_GPIO_WritePin(DRV2603_ENABLE_PORT, DRV2603_ENABLE_PIN, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(htim_drv_pwm, TIM_CHANNEL_1, 0);
    HAL_TIM_PWM_Start(htim_drv_pwm, TIM_CHANNEL_1);
    return 0;
}

void drv2603_enable(uint8_t enable)
{
    HAL_GPIO_WritePin(DRV2603_ENABLE_PORT, DRV2603_ENABLE_PIN, (enable != 0) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}


// NOTE: WILL NEED TO ADJUST timer prescaler, seems to affect the vibration
uint8_t drv2603_set_strength_lra(uint8_t strength_percent)
{
    if (htim_drv_pwm == NULL)
    {
        return EINVAL;
    }

    if (strength_percent > 100)
    {
        strength_percent = 100;
    }

    // lra mode: vout(rms) = 2.2(duty cycle/50) - 1
    // max duty cycle = 77%, full breaking <50%
    // PWM = 50 + (strength * (77 - 50) / 100)
    uint32_t pwm_duty_scaled = 5000 + ((uint32_t)strength_percent * 27);
    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(htim_drv_pwm);

    // compare value: (ARR * PWM_Duty) / 10000
    uint32_t compare_value = (arr * pwm_duty_scaled) / 10000;
    __HAL_TIM_SET_COMPARE(htim_drv_pwm, TIM_CHANNEL_1, compare_value);

    return 0;
}

uint8_t drv2603_set_strength_erm(uint8_t strength_percent)
{
    if (htim_drv_pwm == NULL)
    {
        return EINVAL;
    }

    if (strength_percent > 100)
    {
        strength_percent = 100;
    }

    // erm mode: strength 0-100% to PWM duty cycle 50%-91%
    // 50% = no rotation
    // 91% ~ 3.0V output limit on 3.3V supply
    // PWM = 50 + (strength * (91 - 50) / 100) = 50 + (strength * 41 / 100)
    uint32_t pwm_duty_scaled = 5000 + ((uint32_t)strength_percent * 41);

    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(htim_drv_pwm);
    // compare value: (ARR * PWM_Duty) / 10000
    uint32_t compare_value = (arr * pwm_duty_scaled) / 10000;

    __HAL_TIM_SET_COMPARE(htim_drv_pwm, TIM_CHANNEL_1, compare_value);

    return 0;
}
