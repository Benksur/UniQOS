/**
 * @file contacts.h
 * @brief Contacts list page
 * @ingroup ui_pages
 *
 * Provides the contacts list interface for browsing and managing contact
 * records. Displays a scrollable list of contacts with search and selection
 * capabilities.
 */

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
#include "contact_details.h"

/**
 * @ingroup ui_pages
 * @brief Create the contacts page
 * @return Pointer to the contacts page structure
 *
 * Creates and initializes a new contacts page displaying all stored
 * contacts in a scrollable list with search functionality.
 */
Page *contacts_page_create();

#endif