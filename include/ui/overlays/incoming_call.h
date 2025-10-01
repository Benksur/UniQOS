#ifndef INCOMING_CALL_H
#define INCOMING_CALL_H

#include "screen.h"
#include "input.h"
#include <string.h>
#include <stdlib.h>

// Callback type for call actions
typedef void (*IncomingCallCallback)(int action, void *user_data);

// Action constants
#define INCOMING_CALL_ACTION_PICKUP 0
#define INCOMING_CALL_ACTION_HANGUP 1

Page *incoming_call_overlay_create(const char *phone_number, IncomingCallCallback callback, void *user_data);

#endif // INCOMING_CALL_H
