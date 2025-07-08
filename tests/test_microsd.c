#include <string.h>
#include <stdio.h>
#include "gpio.h"
#include "sdmmc.h"
#include "fatfs.h"
#include "stm32_config.h"
#include "main.h"

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_SDMMC1_SD_Init();
    MX_FATFS_Init();
    FATFS FatFs;
    FIL Fil;
    FRESULT FR_Status;
    FATFS *FS_Ptr;
    UINT RWC, WWC; // Read/Write Word Counter
    DWORD FreeClusters;
    uint32_t TotalSize, FreeSpace;
    char RW_Buffer[200];

    //------------------[ Mount The SD Card ]--------------------
    FR_Status = f_mount(&FatFs, SDPath, 1);
    if (FR_Status != FR_OK)
    {
        DEBUG_PRINTF("Error! While Mounting SD Card, Error Code: (%i)\r\n", FR_Status);
        // return 1;
    }
    DEBUG_PRINTF("SD Card Mounted Successfully! \r\n\n");
    //------------------[ Get & Print The SD Card Size & Free Space ]--------------------
    f_getfree("", &FreeClusters, &FS_Ptr);
    TotalSize = (uint32_t)((FS_Ptr->n_fatent - 2) * FS_Ptr->csize * 0.5);
    FreeSpace = (uint32_t)(FreeClusters * FS_Ptr->csize * 0.5);
    DEBUG_PRINTF("Total SD Card Size: %lu Bytes\r\n", TotalSize);
    DEBUG_PRINTF("Free SD Card Space: %lu Bytes\r\n\n", FreeSpace);
    //------------------[ Open A Text File For Write & Write Data ]--------------------
    // Open the file
    FR_Status = f_open(&Fil, "MyTextFile.txt", FA_WRITE | FA_READ | FA_CREATE_ALWAYS);
    if (FR_Status != FR_OK)
    {
        DEBUG_PRINTF("Error! While Creating/Opening A New Text File, Error Code: (%i)\r\n", FR_Status);
        return 1;
    }
    DEBUG_PRINTF("Text File Created & Opened! Writing Data To The Text File..\r\n\n");
    // (1) Write Data To The Text File [ Using f_puts() Function ]
    f_puts("Hello! From STM32 To SD Card Over SDMMC, Using f_puts()\n", &Fil);
    // (2) Write Data To The Text File [ Using f_write() Function ]
    strcpy(RW_Buffer, "Hello! From STM32 To SD Card Over SDMMC, Using f_write()\r\n");
    f_write(&Fil, RW_Buffer, strlen(RW_Buffer), &WWC);
    // Close The File
    f_close(&Fil);
    //------------------[ Open A Text File For Read & Read Its Data ]--------------------
    // Open The File
    FR_Status = f_open(&Fil, "MyTextFile.txt", FA_READ);
    if (FR_Status != FR_OK)
    {
        DEBUG_PRINTF("Error! While Opening (MyTextFile.txt) File For Read.. \r\n");
        return 1;
    }
    // (1) Read The Text File's Data [ Using f_gets() Function ]
    f_gets(RW_Buffer, sizeof(RW_Buffer), &Fil);
    DEBUG_PRINTF("Data Read From (MyTextFile.txt) Using f_gets():%s", RW_Buffer);
    // (2) Read The Text File's Data [ Using f_read() Function ]
    f_read(&Fil, RW_Buffer, f_size(&Fil), &RWC);
    DEBUG_PRINTF("Data Read From (MyTextFile.txt) Using f_read():%s", RW_Buffer);
    // Close The File
    f_close(&Fil);
    DEBUG_PRINTF("File Closed! \r\n\n");
    //------------------[ Open An Existing Text File, Update Its Content, Read It Back ]--------------------
    // (1) Open The Existing File For Write (Update)
    FR_Status = f_open(&Fil, "MyTextFile.txt", FA_OPEN_EXISTING | FA_WRITE);
    FR_Status = f_lseek(&Fil, f_size(&Fil)); // Move The File Pointer To The EOF (End-Of-File)
    if (FR_Status != FR_OK)
    {
        DEBUG_PRINTF("Error! While Opening (MyTextFile.txt) File For Update.. \r\n");
        return 1;
    }
    // (2) Write New Line of Text Data To The File
    FR_Status = f_puts("This New Line Was Added During File Update!\r\n", &Fil);
    f_close(&Fil);
    memset(RW_Buffer, '\0', sizeof(RW_Buffer)); // Clear The Buffer
    // (3) Read The Contents of The Text File After The Update
    FR_Status = f_open(&Fil, "MyTextFile.txt", FA_READ); // Open The File For Read
    f_read(&Fil, RW_Buffer, f_size(&Fil), &RWC);
    DEBUG_PRINTF("Data Read From (MyTextFile.txt) After Update:\r\n%s", RW_Buffer);


    //------------------[ Test Complete! Unmount The SD Card ]--------------------
    FR_Status = f_mount(NULL, "", 0);
    if (FR_Status != FR_OK)
    {
        DEBUG_PRINTF("Error! While Un-mounting SD Card, Error Code: (%i)\r\n", FR_Status);
    }
    else
    {
        DEBUG_PRINTF("\r\nSD Card Un-mounted Successfully! \r\n");
    }

    while (1)
    {
        HAL_Delay(1000);
    }
}
