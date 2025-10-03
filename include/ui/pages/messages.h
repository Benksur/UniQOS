#ifndef MESSAGES_H
#define MESSAGES_H

#include "screen.h"
#include "display.h"
#include "tile.h"
#include "theme.h"
#include "bottom_bar.h"
#include "sms_types.h"
#include "new_sms.h"
#include <stdlib.h>
#include <string.h>

#define MAX_MESSAGE_LENGTH 255

typedef struct
{
    char sender[SMS_MAX_PHONE_LENGTH + 1];
    char message[MAX_MESSAGE_LENGTH];
} MessagePageState;

Page *messages_page_create(MessagePageState state);

#endif
