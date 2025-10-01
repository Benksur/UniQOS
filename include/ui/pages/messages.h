#ifndef MESSAGES_H
#define MESSAGES_H

#include "screen.h"
#include "display.h"
#include "tile.h"
#include "theme.h"
#include "bottom_bar.h"
#include <stdlib.h>
#include <string.h>

#define MAX_MESSAGE_LENGTH 255

typedef struct
{
    char sender[10];
    char message[MAX_MESSAGE_LENGTH];
} MessagePageState;

Page *messages_page_create(MessagePageState state);

#endif
