#pragma once
#include <stdint.h>

typedef struct {
    void (*set_volume)(uint8_t volume);
    void (*mute)(uint8_t enable);
} IMicrophone_t;

typedef struct {
    void (*set_volume)(uint8_t volume);
    void (*mute)(uint8_t enable);
    IMicrophone_t mic;
} ISpeaker_t;

typedef struct {
    void (*set_volume)(uint8_t volume);
    void (*mute)(uint8_t enable);
    IMicrophone_t mic;
} IHeadphones_t;


typedef struct {
    void (*init)(void);
    void (*sleep)(uint8_t enable);
    ISpeaker_t speaker;
    IHeadphones_t headphones;
} IAudioDriver_t;