/**
 * @file option_overlay.h
 * @brief Generic option selection overlay
 * @ingroup ui_overlays
 *
 * Provides a generic option selection overlay for displaying lists of
 * selectable options with a header. Commonly used for menus and settings.
 */

#ifndef OPTION_OVERLAY_H
#define OPTION_OVERLAY_H

#include "screen.h"
#include "option_row.h"
#include "input.h"
#include <string.h>
#include <stdlib.h>

/**
 * @brief Callback function type for option selection
 * @ingroup ui_overlays
 * @param selected_idx Index of the selected option
 * @param user_data User-provided data pointer
 */
typedef void (*OptionOverlayCallback)(int selected_idx, void *user_data);

/**
 * @ingroup ui_overlays
 * @brief Create an option selection overlay
 * @param header Header text to display at the top of the overlay
 * @param options Array of option strings to display
 * @param num_options Number of options in the array
 * @param callback Callback function to handle option selection
 * @param user_data User data to pass to the callback
 * @return Pointer to the option overlay page
 *
 * Creates and initializes an option selection overlay with the specified
 * header and list of options. Users can navigate and select from the options.
 */
Page *option_overlay_page_create(const char *header, const char **options, int num_options, OptionOverlayCallback callback, void *user_data);

#endif // OPTION_OVERLAY_H
