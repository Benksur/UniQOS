#include "theme.h"

Theme current_theme;

void theme_set_light(void) {
    current_theme.bg_colour        = 0xFFFF;
    current_theme.text_colour     = 0x0000; 
    current_theme.fg_colour       = 0x05F5;
    current_theme.accent_colour   = 0xffe0; 
    current_theme.highlight_colour      = 0x0861;
}

void theme_set_dark(void) {
    current_theme.bg_colour        = 0x0000;
    current_theme.text_colour     = 0xFFFF;
    current_theme.fg_colour       = 0x05F5;
    current_theme.accent_colour   = 0xffe0;
    current_theme.highlight_colour      = 0xcd39;
}
