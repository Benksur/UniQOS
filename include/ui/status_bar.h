#ifndef STATUS_BAR_H
#define STATUS_BAR_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "rtc.h"
#include "theme.h"
#include "display.h"

// Main status bar drawing function (fills background only on first call)
void draw_status_bar(void);

// Status bar tick function - call this from screen_tick() for automatic time updates
void status_bar_tick(void);

// Manual update functions - only redraw changed elements
void status_bar_update_signal(uint8_t strength);
void status_bar_update_battery(uint8_t level);

// Volume indicator function - shows volume percentage for 5 seconds
void status_bar_show_volume(uint8_t volume);

// Control functions
void status_bar_reset(void);

#endif