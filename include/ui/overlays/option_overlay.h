#ifndef OPTION_OVERLAY_H
#define OPTION_OVERLAY_H

#include "screen.h"
#include "option_row.h"
#include "input.h"
#include <string.h>
#include <stdlib.h>

// Callback type for option selection
typedef void (*OptionOverlayCallback)(int selected_idx, void* user_data);

Page* option_overlay_page_create(const char* header, const char** options, int num_options, OptionOverlayCallback callback, void* user_data);

#endif // OPTION_OVERLAY_H
