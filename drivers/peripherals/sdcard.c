#include <string.h>
#include <stdio.h>
#include "gpio.h"
#include "sdmmc.h"
#include "fatfs.h"
#include "stm32_config.h"
#include "main.h"
#include "errornum.h"

// Main FatFS instance
FATFS FatFs;

// Current open file
FIL File;

// Custer info
DWORD clusters;

uint8_t sdcard_init(void)
{
    FRESULT ret;

    ret = f_mount(&FatFs, SDPath, 1);
    if (ret != FR_OK)
    {
        DEBUG_PRINTF("SD card mount failed with code (%i)\r\n", ret);
        return ENODEV;
    }

    return 0;
}

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
    // TotalSize = (uint32_t)((FS_Ptr->n_fatent - 2) * FS_Ptr->csize * 0.5);

    return 0;
}

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

        // Do something with the names put into string list? 
        // fno.fname until null char
        // other file info here
    }
    return 0;
}