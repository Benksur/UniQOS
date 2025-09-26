#ifndef MIXER_H
#define MIXER_H

#include <stdint.h>
#include "oscillator.h"
#include "i2s.h"
#include "stm32_config.h"

#define MAX_OSCS 4

// Note frequencies (Hz) for one octave
#define NOTE_C  261.63f
#define NOTE_CS 277.18f
#define NOTE_D  293.66f
#define NOTE_DS 311.13f
#define NOTE_E  329.63f
#define NOTE_F  349.23f
#define NOTE_FS 369.99f
#define NOTE_G  392.00f
#define NOTE_GS 415.30f
#define NOTE_A  440.00f
#define NOTE_AS 466.16f
#define NOTE_B  493.88f
#define NOTE_C2 523.25f

#define NOTE_OCTAVE(note, octave) ((note) * (1 << (octave)))

typedef struct {
    Oscillator oscs[MAX_OSCS];
    int count;
} Mixer;

void mixer_init(Mixer *mix);
void mixer_add(Mixer *mix, float freq);
int16_t mixer_next(Mixer *mix);
void play_chord(Mixer *mix, int duration_ms);


#endif 