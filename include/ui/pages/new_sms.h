#ifndef NEW_SMS_H
#define NEW_SMS_H

#include "screen.h"
#include "display.h"
#include "tile.h"
#include "menu_row.h"
#include "theme.h"
#include "input.h"
#include "cursor.h"
#include <stdlib.h>
#include <string.h>

Page *new_sms_page_create(const char *phone_number);

#endif