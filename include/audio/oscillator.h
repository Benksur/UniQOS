/**
 * @file oscillator.h
 * @brief Digital oscillator for audio synthesis
 * @ingroup audio_oscillator
 *
 * Provides digital oscillator functionality for generating
 * sine waves and other waveforms for audio synthesis.
 */

#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <stdint.h>

/** @ingroup audio_oscillator
 *  @brief Sine wave lookup table */
extern int16_t sine[];

/** @ingroup audio_oscillator
 *  @brief Audio sample rate in Hz */
#define SAMPLE_RATE 16000

/** @ingroup audio_oscillator
 *  @brief Length of sine wave lookup table */
#define WAVE_LENGTH 128

/**
 * @brief Oscillator structure
 * @ingroup audio_oscillator
 *
 * Represents a single oscillator with phase tracking.
 */
typedef struct
{
    float phase;     /**< Current phase (0.0 to 1.0) */
    float phase_inc; /**< Phase increment per sample */
} Oscillator;

/**
 * @ingroup audio_oscillator
 * @brief Generate sine wave lookup table
 *
 * Must be called once before using oscillators.
 */
void osc_generate_sine_table();

/**
 * @ingroup audio_oscillator
 * @brief Initialize oscillator with frequency
 * @param osc Pointer to oscillator structure
 * @param freq Frequency in Hz
 */
void osc_init(Oscillator *osc, float freq);

/**
 * @ingroup audio_oscillator
 * @brief Generate next sample from oscillator
 * @param osc Pointer to oscillator structure
 * @return Audio sample (16-bit signed)
 */
int16_t osc_next(Oscillator *osc);

#endif