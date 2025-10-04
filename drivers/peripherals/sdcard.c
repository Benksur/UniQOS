/**
 * @file sdcard.c
 * @brief SD card driver implementation using FatFS
 *
 * This driver provides SD card functionality using the FatFS file system.
 * It supports mounting, unmounting, file operations, and space management.
 */

#include <string.h>
#include <stdio.h>
#include "gpio.h"
#include "sdmmc.h"
#include "fatfs.h"
#include "stm32_config.h"
#include "main.h"
#include "errornum.h"
#include "sdcard.h"


// Main FatFS instance
FATFS FatFs;
FIL File;
DWORD clusters;

const char config_file[] = "config.ini";

/**
 * @brief Initialize and mount the SD card
 * @return 0 on success, ENODEV if mount fails
 */
uint8_t sdcard_init(void)
{
    FRESULT ret;

    // may need to play with the card detect pin
    ret = f_mount(&FatFs, SDPath, 1);
    if (ret != FR_OK)
    {
        DEBUG_PRINTF("SD card mount failed with code (%i)\r\n", ret);
        return ENODEV;
    }

    return 0;
}

/**
 * @brief Unmount and deinitialize the SD card
 * @return 0 on success, EIO if unmount fails
 */
uint8_t sdcard_deinit(void)
{
    FRESULT ret;

    ret = f_mount(NULL, "", 0);
    if (ret != FR_OK)
    {
        DEBUG_PRINTF("SD car unmount failed with Code: (%i)\r\n", ret);
        return EIO;
    }

    return 0;
}

/**
 * @brief Get free space on SD card
 * @param free_space Pointer to store free space in KB
 * @return 0 on success, EIO on error
 */
uint8_t sdcard_get_space(uint32_t *free_space)
{
    FRESULT ret;
    FATFS *FS_Ptr;

    ret = f_getfree("", &clusters, &FS_Ptr);

    if (ret != FR_OK)
    {
        DEBUG_PRINTF("SD card could not get space with code: (%i)\r\n", ret);
        return EIO;
    }

    *free_space = (uint32_t)(clusters * FS_Ptr->csize * 0.5);
    // totalsize = (uint32_t)((fs_ptr->n_fatent - 2) * fs_ptr->csize * 0.5);

    return 0;
}

/**
 * @brief List files in a directory on SD card
 * @param path Directory path to list files from
 * @return 0 on success, EIO on error
 */
uint8_t sdcard_list_files(char *path)
{
    DIR dir;
    UINT BytesWritten;
    char string[128];
    FRESULT ret;

    ret = f_opendir(&dir, path);

    if (ret != FR_OK)
    {
        DEBUG_PRINTF("SD card could not get files in dir: (%i)\r\n", ret);
        return EIO;
    }

    while (1)
    {
        FILINFO fno;

        ret = f_readdir(&dir, &fno);

        if ((ret != FR_OK) || (fno.fname[0] == 0))
            break;

        // do something with the names put into string list?
        // fno.fname until null char
        // other file info here
    }
    return 0;
}

/**
 * @brief Load icon data from SD card
 * @param icon Icon type to load
 * @param buff Buffer to store icon data
 * @param len Maximum buffer length
 * @return 0 on success, error code on failure
 */
uint8_t sdcard_get_icon(enum Icons icon, uint8_t *buff, uint32_t len)
{
    char folder_name[MAX_FILENAME_LEN];
    char filename[MAX_FILENAME_LEN];
    char path[MAX_FILENAME_LEN * 2 + 1];
    UINT bytes_read;

    FRESULT res;
    int ret;

    // i believe these are case sensitive be warned
    ini_gets("Icons", "Folder", DEFAULT_ICON_FOLDER, folder_name, MAX_FILENAME_LEN, config_file);

    switch (icon)
    {
    case LOGO:
        ini_gets("Icons", "Logo", "uniq_logo", filename, MAX_FILENAME_LEN, config_file);
        break;

    default:
        return EINVAL;
    }

    ret = snprintf(path, sizeof(path), "%s/%s", folder_name, filename);
    if (ret < 0 || ret >= sizeof(path))
    {
        return E2BIG;
    }

    res = f_open(&File, path, FA_READ);
    if (res != FR_OK)
    {
        return ENOENT;
    }

    if (f_size(&File) > sizeof(buff))
    {
        f_close(&File);
        return E2BIG;
    }

    res = f_read(&File, buff, len, &bytes_read);
    if (res != FR_OK)
    {
        f_close(&File);
        return EIO;
    }

    f_close(&File);

    return 0;
}