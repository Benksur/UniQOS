#ifndef INCOMING_TEXT_H
#define INCOMING_TEXT_H

#include "screen.h"
#include "input.h"
#include "cursor.h"
#include "theme.h"
#include "display.h"
#include "tile.h"
#include <string.h>
#include <stdlib.h>

// Callback type for text actions
typedef void (*IncomingTextCallback)(int action, void *user_data);

// Action constants
#define INCOMING_TEXT_ACTION_OPEN 0
#define INCOMING_TEXT_ACTION_CLOSE 1

Page *incoming_text_overlay_create(const char *sender_number, IncomingTextCallback callback, void *user_data);

#endif // INCOMING_TEXT_H
