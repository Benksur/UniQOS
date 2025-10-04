/**
 * @file drv2603.c
 * @brief DRV2603 haptic motor driver implementation
 *
 * This driver provides PWM control for the DRV2603 haptic motor driver.
 */

#include "drv2603.h"
#include "errornum.h"

uint8_t drv2603_init()
{
    HAL_GPIO_WritePin(DRV2603_ENABLE_PORT, DRV2603_ENABLE_PIN, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(DRV2603_TIM, DRV2603_TIM_CHANEL, 0);
    HAL_TIM_PWM_Start(DRV2603_TIM, DRV2603_TIM_CHANEL);
    return 0;
}

/**
 * @brief Enable or disable the DRV2603 haptic motor driver
 * @param enable 1 to enable, 0 to disable
 */
void drv2603_enable(uint8_t enable)
{
    HAL_GPIO_WritePin(DRV2603_ENABLE_PORT, DRV2603_ENABLE_PIN, (enable != 0) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
 * @brief Set vibration strength for LRA (Linear Resonant Actuator) motor
 * @param strength_percent Vibration strength (0-100%)
 * @return 0 on success
 * @note Timer prescaler may need adjustment for optimal vibration
 */
uint8_t drv2603_set_strength_lra(uint8_t strength_percent)
{
    if (strength_percent > 100)
    {
        strength_percent = 100;
    }

    // lra mode: vout(rms) = 2.2(duty cycle/50) - 1
    // max duty cycle = 77%, full breaking <50%
    // pwm = 50 + (strength * (77 - 50) / 100)
    uint32_t pwm_duty_scaled = 5000 + ((uint32_t)strength_percent * 27);
    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(DRV2603_TIM);

    // compare value: (arr * pwm_duty) / 10000
    uint32_t compare_value = (arr * pwm_duty_scaled) / 10000;
    __HAL_TIM_SET_COMPARE(DRV2603_TIM, DRV2603_TIM_CHANEL, compare_value);

    return 0;
}

/**
 * @brief Set vibration strength for ERM (Eccentric Rotating Mass) motor
 * @param strength_percent Vibration strength (0-100%)
 * @return 0 on success
 */
uint8_t drv2603_set_strength_erm(uint8_t strength_percent)
{
    if (strength_percent > 100)
    {
        strength_percent = 100;
    }

    // erm mode: strength 0-100% to pwm duty cycle 50%-91%
    // 50% = no rotation
    // 91% ~ 3.0v output limit on 3.3v supply
    // pwm = 50 + (strength * (91 - 50) / 100) = 50 + (strength * 41 / 100)
    uint32_t pwm_duty_scaled = 5000 + ((uint32_t)strength_percent * 41);

    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(DRV2603_TIM);
    // compare value: (arr * pwm_duty) / 10000
    uint32_t compare_value = (arr * pwm_duty_scaled) / 10000;

    __HAL_TIM_SET_COMPARE(DRV2603_TIM, DRV2603_TIM_CHANEL, compare_value);

    return 0;
}
