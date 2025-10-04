/**
 * @file sdcard.h
 * @brief SD card interface driver
 * @ingroup sdcard_driver
 *
 * Provides functions for SD card initialization, file operations, and
 * icon loading. Uses the minIni library for configuration file handling.
 */

#ifndef SDCARD_H_
#define SDCARD_H_
#include "stm32_config.h"
#include "minIni.h"

/** @ingroup sdcard_driver
 *  @brief Default folder path for icon files */
#define DEFAULT_ICON_FOLDER "icons"

/** @ingroup sdcard_driver
 *  @brief Maximum filename length for SD card operations */
#define MAX_FILENAME_LEN 64

/**
 * @brief Icon enumeration for predefined icons
 * @ingroup sdcard_driver
 */
enum Icons
{
    LOGO, /**< System logo icon */
};

/**
 * @ingroup sdcard_driver
 * @brief Initialize the SD card
 * @return 0 on success, error code otherwise
 *
 * Initializes the SD card interface and mounts the filesystem.
 * Must be called before any other SD card operations.
 */
uint8_t sdcard_init(void);

/**
 * @ingroup sdcard_driver
 * @brief Get available free space on SD card
 * @param free_space Pointer to receive free space in bytes
 * @return 0 on success, error code otherwise
 *
 * Retrieves the amount of free space available on the SD card.
 */
uint8_t sdcard_get_space(uint32_t *free_space);

/**
 * @ingroup sdcard_driver
 * @brief List files in a directory
 * @param path Directory path to list
 * @return 0 on success, error code otherwise
 *
 * Lists all files in the specified directory path.
 */
uint8_t sdcard_list_files(char *path);

/**
 * @ingroup sdcard_driver
 * @brief Load an icon from SD card
 * @param icon Icon type to load
 * @param buff Buffer to store icon data
 * @param len Maximum buffer length
 * @return 0 on success, error code otherwise
 *
 * Loads the specified icon from the SD card into the provided buffer.
 */
uint8_t sdcard_get_icon(enum Icons icon, uint8_t *buff, uint32_t len);

#endif