/**
 * @file iaudio_driver.h
 * @brief Audio driver interface definitions
 * @ingroup audio_drivers
 *
 * Defines the abstract interface for audio drivers. This allows the system
 * to work with different audio codecs through a common interface.
 */

#pragma once
#include <stdint.h>

/**
 * @brief Microphone interface structure
 * @ingroup audio_drivers
 *
 * Defines the interface for microphone control operations.
 */
typedef struct
{
    void (*set_volume)(uint8_t volume); /**< Set microphone volume (0-100%) */
    void (*mute)(uint8_t enable);       /**< Mute/unmute microphone (1=mute, 0=unmute) */
} IMicrophone_t;

/**
 * @brief Speaker interface structure
 * @ingroup audio_drivers
 *
 * Defines the interface for speaker control operations.
 */
typedef struct
{
    void (*set_volume)(uint8_t volume); /**< Set speaker volume (0-100%) */
    void (*mute)(uint8_t enable);       /**< Mute/unmute speaker (1=mute, 0=unmute) */
    IMicrophone_t mic;                  /**< Microphone interface */
} ISpeaker_t;

/**
 * @brief Headphones interface structure
 * @ingroup audio_drivers
 *
 * Defines the interface for headphone control operations.
 */
typedef struct
{
    void (*set_volume)(uint8_t volume); /**< Set headphone volume (0-100%) */
    void (*mute)(uint8_t enable);       /**< Mute/unmute headphones (1=mute, 0=unmute) */
    IMicrophone_t mic;                  /**< Headphone microphone interface */
} IHeadphones_t;

/**
 * @brief Main audio driver interface structure
 * @ingroup audio_drivers
 *
 * Defines the complete interface for audio driver implementations.
 * All audio drivers must implement this interface.
 */
typedef struct
{
    void (*init)(void);            /**< Initialize the audio driver */
    void (*sleep)(uint8_t enable); /**< Put driver to sleep/wake up (1=sleep, 0=wake) */
    ISpeaker_t speaker;            /**< Speaker control interface */
    IHeadphones_t headphones;      /**< Headphone control interface */
} IAudioDriver_t;