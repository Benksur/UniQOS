/**
 * @file contact_details.h
 * @brief Contact details view page
 * @ingroup ui_pages
 *
 * Provides the contact details interface for viewing and managing individual
 * contact information. Displays contact data and provides options for calling,
 * messaging, and editing contacts.
 */

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

/**
 * @ingroup ui_pages
 * @brief Create a contact details page
 * @param contact Contact record to display
 * @return Pointer to the contact details page structure
 *
 * Creates and initializes a new contact details page for the specified
 * contact. The page displays contact information and provides options
 * for calling, messaging, and editing the contact.
 */
Page *contact_details_page_create(ContactRecord contact);

#endif