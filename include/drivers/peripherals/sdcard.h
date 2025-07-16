#ifndef SDCARD_H_
#define SDCARD_H_
#include "stm32_config.h"
#include "minIni.h"

#define DEFAULT_ICON_FOLDER "icons"
#define MAX_FILENAME_LEN 64

enum Icons {
    LOGO,
};

uint8_t sdcard_init(void);
uint8_t sdcard_get_space(uint32_t *free_space);
uint8_t sdcard_list_files(char *path);
uint8_t sdcard_get_icon(enum Icons icon, uint8_t *buff);


#endif