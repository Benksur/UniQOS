#include "st7789v.h"
#include "display.h"
#include "theme.h"
#include "screen.h"
#include "pages/menu.h"
#include "gpio.h"
#include "spi.h"
#include "tile.h"
#include "keypad.h"

#include "nau88c22.h"
#include "tick_sound.h"
#include "i2c.h"
#include "i2s.h"
#include "rtc.h"

#include "mixer.h"

#include <stdbool.h>
#include <math.h>

// Note frequencies (Hz) for one octave
#define NOTE_C 261.63f
#define NOTE_CS 277.18f
#define NOTE_D 293.66f
#define NOTE_DS 311.13f
#define NOTE_E 329.63f
#define NOTE_F 349.23f
#define NOTE_FS 369.99f
#define NOTE_G 392.00f
#define NOTE_GS 415.30f
#define NOTE_A 440.00f
#define NOTE_AS 466.16f
#define NOTE_B 493.88f
#define NOTE_C2 523.25f
// Helper macro to shift octave
#define NOTE_OCTAVE(note, octave) ((note) * powf(2.0f, (octave)))

void MPU_Config(void);
void SystemClock_Config(void);

int main(void)
{
    MPU_Config();
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_SPI4_Init();
    MX_I2C1_Init();
    MX_I2S1_Init();
    MX_RTC_Init();

    // MX_TIM2_Init();
    //   HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    //   htim2.Instance->CCR3 = 80;

    uint8_t status;
    static const IAudioDriver_t *codec = NULL;
    codec = nau88c22_get_driver();
    codec->init();
    codec->speaker.mute(false);
    codec->speaker.set_volume(100);

    display_init();
    keypad_init();

    HAL_Delay(1000);

    theme_set_dark();
    display_fill_rect(0, 0, 240, 25, current_theme.fg_colour);

    char time_buffer[15] = {0};
    RTC_DateTypeDef sDate;
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    // RTC_TimeTypeDef sTime;
    // HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    // sprintf(time_buffer, "%02d:%02d:%02d", sTime.Hours, sTime.Minutes, sTime.Seconds);
    sprintf(time_buffer, "20%02d-%02d-%02d", sDate.Year, sDate.Month, sDate.Date);
    display_draw_string(10, 5, time_buffer, current_theme.bg_colour, current_theme.fg_colour, 2);

    screen_init(&menu_page);
    mark_all_tiles_dirty();
    screen_tick();
    // draw_grid();
    osc_generate_sine_table();
    Mixer chord;

    //   mixer_add(&chord, 329.63f);
    //   mixer_add(&chord, 392.00f);

    int octave_shift = 0;

    while (1)
    {
        // Update keypad states and handle all inputs
        for (int i = 0; i < 24; i++)
        {
            keypad_update_states();

            // Check all buttons and handle input events
            for (int button_idx = 0; button_idx < keypad_get_button_count(); button_idx++)
            {
                if (keypad_is_button_pressed(button_idx))
                {
                    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
                    input_event_t event = keypad_get_button_event(button_idx);
                    switch (event)
                    {
                    case INPUT_DPAD_UP:
                        octave_shift++;
                        break;
                    case INPUT_DPAD_DOWN:
                        octave_shift--;
                        break;
                    case INPUT_KEYPAD_0:

                        // Phrase 1: ^E ^E ^E ^C ^E ^G G
                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_E, 0)); // ^E
                        play_chord(&chord, 200); // short

                        mixer_init(&chord);
                        play_chord(&chord, 150); // short rest

                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_E, 0));
                        play_chord(&chord, 200); // short

                        mixer_init(&chord);
                        play_chord(&chord, 150); // short rest

                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_E, 0));
                        play_chord(&chord, 200); // long

                        mixer_init(&chord);
                        play_chord(&chord, 300); // short rest

                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_C, 0)); // ^C
                        play_chord(&chord, 300); // long

                        mixer_init(&chord);
                        play_chord(&chord, 200); // short rest

                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_E, 0));
                        play_chord(&chord, 300); // long

                        mixer_init(&chord);
                        play_chord(&chord, 150); // short rest

                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_G, 0)); // G
                        play_chord(&chord, 300); // long

                        mixer_init(&chord);
                        play_chord(&chord, 150); // short rest

                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_G, -1)); // G
                        play_chord(&chord, 400); // held

                        mixer_init(&chord);
                        play_chord(&chord, 200); // rest

                        // Phrase 2: ^C G E
                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_C, 0)); // ^C
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_G, -1));
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_E, -1));
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        play_chord(&chord, 200);

                        // Phrase 3: A B Bb A
                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_A, -1));
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_B, -1));
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_AS, -1)); // Bb = A#
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_A, -1));
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        play_chord(&chord, 200);

                        // Phrase 4: G ^E ^G ^A
                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_G, -1));
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_E, 0)); // ^E
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_G, 0)); // ^G
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_A, 0)); // ^A
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        play_chord(&chord, 200);

                        // Phrase 5: ^F ^G ^E ^C ^D B
                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_F, 0)); // ^F
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_G, 0)); // ^G
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_E, 0)); // ^E
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_C, 0)); // ^C
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_D, 0)); // ^D
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_B, -1));
                        play_chord(&chord, 200);

                        mixer_init(&chord);
                        play_chord(&chord, 200);

                        break;

                    case INPUT_KEYPAD_1:
                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_C, octave_shift));
                        play_chord(&chord, 200);
                        break;
                    case INPUT_KEYPAD_2:
                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_D, octave_shift));
                        play_chord(&chord, 200);
                        break;
                    case INPUT_KEYPAD_3:
                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_E, octave_shift));
                        play_chord(&chord, 200);
                        break;
                    case INPUT_KEYPAD_4:
                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_F, octave_shift));
                        play_chord(&chord, 200);
                        break;
                    case INPUT_KEYPAD_5:
                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_G, octave_shift));
                        play_chord(&chord, 200);
                        break;
                    case INPUT_KEYPAD_6:
                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_A, octave_shift));
                        play_chord(&chord, 200);
                        break;
                    case INPUT_KEYPAD_7:
                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_B, octave_shift));
                        play_chord(&chord, 200);
                        break;
                    case INPUT_KEYPAD_8:
                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_C2, octave_shift));
                        play_chord(&chord, 200);
                        break;
                    case INPUT_KEYPAD_9:
                        mixer_init(&chord);
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_C, octave_shift));
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_E, octave_shift));
                        mixer_add(&chord, NOTE_OCTAVE(NOTE_G, octave_shift));
                        play_chord(&chord, 200);
                        break;
                    default:
                        break;
                    }
                }
                screen_tick();
            }
        }
    }
}

/**
 * @brief  The application entry point.
 * @retval int
 */

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Supply configuration update enable
     */
    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

    /** Configure the main internal regulator output voltage
     */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
    {
    }

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = 64;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 30;
    RCC_OscInitStruct.PLL.PLLP = 1;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void)
{
    MPU_Region_InitTypeDef MPU_InitStruct = {0};

    /* Disables the MPU */
    HAL_MPU_Disable();

    /** Initializes and configures the Region and the memory to be protected
     */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER0;
    MPU_InitStruct.BaseAddress = 0x0;
    MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
    MPU_InitStruct.SubRegionDisable = 0x87;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    /* Enables the MPU */
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

    if (htim->Instance == TIM1)
    {
        HAL_IncTick();
    }
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    // Add LED toggle for visual feedback
    while (1)
    {
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0); // Assuming LED is on PB0
        HAL_Delay(100);                        // Blink rate
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
