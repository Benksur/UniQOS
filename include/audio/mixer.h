#ifndef MIXER_H
#define MIXER_H

#include <stdint.h>
#include "oscillator.h"
#include "i2s.h"
#include "stm32_config.h"

#define MAX_OSCS 4

typedef struct {
    Oscillator oscs[MAX_OSCS];
    int count;
} Mixer;

void mixer_init(Mixer *mix);
void mixer_add(Mixer *mix, float freq);
int16_t mixer_next(Mixer *mix);
void play_chord(Mixer *mix, int duration_ms);


#endif 