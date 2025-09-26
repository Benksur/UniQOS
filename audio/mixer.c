#include "mixer.h"




void mixer_init(Mixer *mix) {
    mix->count = 0;
}

void mixer_add(Mixer *mix, float freq) {
    if (mix->count < MAX_OSCS) {
        osc_init(&mix->oscs[mix->count++], freq);
    }
}

int16_t mixer_next(Mixer *mix) {
    int32_t sum = 0;
    for (int i = 0; i < mix->count; i++) {
        sum += osc_next(&mix->oscs[i]);
    }
    return (int16_t)(sum / mix->count);  // normalize
}

void play_chord(Mixer *mix, int duration_ms) {
    int total_samples = SAMPLE_RATE * duration_ms / 1000;
    for (int i = 0; i < total_samples; i++) {
        int16_t sample = mixer_next(mix);
        HAL_I2S_Transmit(&AUDIO_I2S_HANDLE, (uint16_t*)&sample, 1, HAL_MAX_DELAY);
    }
}

