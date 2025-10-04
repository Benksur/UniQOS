/**
 * @file calculator.h
 * @brief Calculator application page
 * @ingroup ui_pages
 *
 * Provides the calculator application interface with basic arithmetic operations.
 * Users can perform calculations using the keypad input.
 */

#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "screen.h"

/**
 * @ingroup ui_pages
 * @brief Create the calculator page
 * @return Pointer to the calculator page structure
 *
 * Creates and initializes a new calculator page with a clean display
 * ready for user input and calculations.
 */
Page *calculator_page_create();

#endif