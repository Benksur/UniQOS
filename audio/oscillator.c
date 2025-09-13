#include "oscillator.h"
#include <math.h>

#define WAVE_LENGTH 1024
#define M_PI 3.14159265358979323846

int16_t sine[WAVE_LENGTH];

// Generate the sine table at startup
void osc_generate_sine_table() {
    for (int i = 0; i < WAVE_LENGTH; ++i) {
        sine[i] = (int16_t)(32767 * sinf(2.0f * M_PI * i / WAVE_LENGTH));
    }
}

void osc_init(Oscillator *osc, float freq) {
    osc->phase = 0.0f;
    osc->phase_inc = (float)WAVE_LENGTH * freq / SAMPLE_RATE;
}

int16_t osc_next(Oscillator *osc) {
    int idx = (int)osc->phase;
    int next_idx = (idx + 1) % WAVE_LENGTH;
    float frac = osc->phase - idx;
    float sample = (1.0f - frac) * sine[idx] + frac * sine[next_idx];
    // Apply higher gain to boost amplitude (3.0x, clamp to int16_t range)
    sample *= 6.0f;
    if (sample > 32767) sample = 32767;
    if (sample < -32768) sample = -32768;
    osc->phase += osc->phase_inc;
    if (osc->phase >= WAVE_LENGTH) osc->phase -= WAVE_LENGTH;
    return (int16_t)sample;
}
