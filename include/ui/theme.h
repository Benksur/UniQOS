#ifndef THEME_H
#define THEME_H

#include <stdint.h>

typedef struct {
    uint16_t bg_color;
    uint16_t text_colour;
    uint16_t fg_colour;
    uint16_t accent_colour;
} Theme;

extern Theme current_theme;

void theme_set_default(void);
void theme_set_dark(void);

#endif
