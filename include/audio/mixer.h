/**
 * @file mixer.h
 * @brief Audio mixer for multiple oscillators
 * @ingroup audio_mixer
 *
 * Multi-channel audio mixer that combines multiple oscillators
 * to create complex sounds, chords, and musical effects.
 */

#ifndef MIXER_H
#define MIXER_H

#include <stdint.h>
#include "oscillator.h"
#include "i2s.h"
#include "stm32_config.h"

/** @ingroup audio_mixer
 *  @brief Maximum number of oscillators in mixer */
#define MAX_OSCS 4

/** @ingroup audio_mixer
 *  @brief Note frequencies (Hz) for one octave */
#define NOTE_C 261.63f  /**< C note frequency */
#define NOTE_CS 277.18f /**< C# note frequency */
#define NOTE_D 293.66f  /**< D note frequency */
#define NOTE_DS 311.13f /**< D# note frequency */
#define NOTE_E 329.63f  /**< E note frequency */
#define NOTE_F 349.23f  /**< F note frequency */
#define NOTE_FS 369.99f /**< F# note frequency */
#define NOTE_G 392.00f  /**< G note frequency */
#define NOTE_GS 415.30f /**< G# note frequency */
#define NOTE_A 440.00f  /**< A note frequency */
#define NOTE_AS 466.16f /**< A# note frequency */
#define NOTE_B 493.88f  /**< B note frequency */
#define NOTE_C2 523.25f /**< C note frequency (next octave) */

/** @ingroup audio_mixer
 *  @brief Calculate note frequency for given octave */
#define NOTE_OCTAVE(note, octave) ((note) * (1 << (octave)))

/**
 * @brief Audio mixer structure
 * @ingroup audio_mixer
 *
 * Manages multiple oscillators and mixes their outputs.
 */
typedef struct
{
    Oscillator oscs[MAX_OSCS]; /**< Array of oscillators */
    int count;                 /**< Number of active oscillators */
} Mixer;

/**
 * @ingroup audio_mixer
 * @brief Initialize the audio mixer
 * @param mix Pointer to mixer structure
 */
void mixer_init(Mixer *mix);

/**
 * @ingroup audio_mixer
 * @brief Add an oscillator to the mixer
 * @param mix Pointer to mixer structure
 * @param freq Frequency in Hz for the new oscillator
 */
void mixer_add(Mixer *mix, float freq);

/**
 * @ingroup audio_mixer
 * @brief Generate next sample from all oscillators
 * @param mix Pointer to mixer structure
 * @return Mixed audio sample (16-bit signed)
 */
int16_t mixer_next(Mixer *mix);

/**
 * @ingroup audio_mixer
 * @brief Play a chord for specified duration
 * @param mix Pointer to mixer structure
 * @param duration_ms Duration in milliseconds
 */
void play_chord(Mixer *mix, int duration_ms);

#endif