#ifndef CONTACT_DETAILS_H
#define CONTACT_DETAILS_H

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "screen.h"
#include "display.h"
#include "tile.h"
#include "input.h"
#include "cursor.h"
#include "theme.h"
#include "contacts_bptree.h"
#include "option_row.h"
#include "bottom_bar.h"
#include "new_sms.h"
#include "call.h"

Page* contact_details_page_create(ContactRecord contact);



#endif