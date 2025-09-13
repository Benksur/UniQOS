#ifndef CONTACTS_H
#define CONTACTS_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "screen.h"
#include "tile.h"
#include "cursor.h"
#include "input.h"
#include "contacts_bptree.h"
#include "contact_row.h"

Page* contacts_page_create();

#endif