#ifndef SDCARD_H_
#define SDCARD_H_
#include "stm32_config.h"

uint8_t sdcard_init(void);
uint8_t sdcard_get_space(uint32_t *free_space);
uint8_t sdcard_list_files(char *path);


#endif