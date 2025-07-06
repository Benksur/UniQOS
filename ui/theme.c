#include "theme.h"

Theme current_theme;

void theme_set_light(void) {
    current_theme.bg_color        = 0xFFFF;
    current_theme.text_colour     = 0x0000; 
    current_theme.fg_colour       = 0xFC20;
    current_theme.accent_colour   = 0x881f; 
}

void theme_set_dark(void) {
    current_theme.bg_color        = 0x0000;
    current_theme.text_colour     = 0xFFFF;
    current_theme.fg_colour       = 0xFC20;
    current_theme.accent_colour   = 0x881f;
}
