#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <stdint.h>

extern int16_t sine[];

#define SAMPLE_RATE 16000
#define WAVE_LENGTH 128  // length of sine[]

typedef struct {
    float phase;
    float phase_inc;
} Oscillator;

void osc_init(Oscillator *osc, float freq);
int16_t osc_next(Oscillator *osc);

#endif