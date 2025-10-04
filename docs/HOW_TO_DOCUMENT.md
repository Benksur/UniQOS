# How to Document Code with Doxygen

This guide explains how to add Doxygen documentation to the UniQOS codebase using the existing setup.

## Quick Start

1. **Add file header** to every `.h` and `.c` file
2. **Document functions** with brief descriptions and parameters
3. **Document data structures** with member descriptions
4. **Use `@ingroup` tags** to organize code into modules

## File Headers

Every file should start with a file header:

```
/**
 * @file filename.h
 * @brief Brief description of what this file does
 * @ingroup group_name
 * 
 * Detailed description of the file's purpose and functionality.
 * This can span multiple lines and explain the module's role
 * in the overall system.
 */
```

## Function Documentation

Document every public function:

```
/**
 * @ingroup group_name
 * @brief Brief one-line description
 * @param param1 Description of first parameter
 * @param param2 Description of second parameter
 * @return Description of return value
 * 
 * Detailed description of what the function does, any side effects,
 * and important usage notes.
 */
returntype function_name(type1 param1, type2 param2);
```

## Data Structure Documentation

Document structs and their members:

```
/**
 * @brief Structure description
 * @ingroup group_name
 * 
 * Detailed description of what this structure represents
 * and how it's used in the system.
 */
typedef struct {
    int field1;      /**< Description of field1 */
    char *field2;    /**< Description of field2 */
    bool flag;       /**< Description of flag field */
} my_struct_t;
```

## Enumeration Documentation

Document enums and their values:

```
/**
 * @brief Enum description
 * @ingroup group_name
 * 
 * Description of what this enumeration represents.
 */
typedef enum {
    VALUE_ONE = 0,    /**< Description of VALUE_ONE */
    VALUE_TWO,        /**< Description of VALUE_TWO */
    VALUE_THREE       /**< Description of VALUE_THREE */
} my_enum_t;
```

## Constants and Macros

Document important constants:

```
/** @ingroup group_name
 *  @brief Description of this constant */
#define MY_CONSTANT 42

/** @ingroup group_name
 *  @brief Description of this macro
 *  @param x Description of parameter x */
#define MY_MACRO(x) ((x) * 2)
```

## Module Groups

Use these `@ingroup` tags to organize your code:

### Driver Modules
- `@ingroup drivers` - Main drivers group
- `@ingroup audio_drivers` - Audio-related drivers
- `@ingroup display_drivers` - Display-related drivers
- `@ingroup modem_drivers` - Cellular modem drivers
- `@ingroup peripheral_drivers` - Other peripheral drivers
- `@ingroup power_drivers` - Power management drivers

### Kernel Modules
- `@ingroup kernel` - Main kernel group
- `@ingroup kernel_core` - Core kernel functionality
- `@ingroup tasks` - System tasks
- `@ingroup data_structures` - Core data structures
- `@ingroup memory` - Memory management

### UI Modules
- `@ingroup ui` - Main UI group
- `@ingroup ui_core` - Core UI components
- `@ingroup ui_components` - Reusable UI widgets
- `@ingroup ui_pages` - Application pages
- `@ingroup ui_overlays` - Modal dialogs
- `@ingroup ui_input` - Input handling

### Audio Modules
- `@ingroup audio` - Main audio group
- `@ingroup audio_mixer` - Audio mixing
- `@ingroup audio_oscillator` - Tone generation

### Test Modules
- `@ingroup tests` - Test programs

## Best Practices

### 1. Be Concise but Complete
- Use `@brief` for one-line descriptions
- Add detailed descriptions when needed
- Include units in descriptions (e.g., "timeout in milliseconds")

### 2. Document Parameters and Returns
- Always describe what each parameter does
- Specify units, ranges, and special values
- Note any side effects or error conditions

### 3. Use Inline Comments for Members
```
typedef struct {
    int voltage;     /**< Battery voltage in millivolts */
    int current;     /**< Current draw in milliamperes */
    bool charging;   /**< True if battery is charging */
} battery_status_t;
```

### 4. Cross-Reference Related Code
```
/**
 * @brief Initialize the display
 * @see display_fill() for filling the screen
 * @see display_draw_string() for drawing text
 */
void display_init(void);
```

### 5. Mark Deprecated Items
```
/**
 * @deprecated Use new_function() instead
 * @brief Old function description
 */
void old_function(void);
```

## Building Documentation

To generate the Doxygen documentation:

```bash
cd /home/benksur/Documents/uni/reit4841/UniQOS
doxygen Doxyfile
```

The generated documentation will be available in:
- **HTML**: `docs/html/index.html`
- **LaTeX**: `docs/latex/`

## Common Patterns

### Driver Functions
```
/**
 * @ingroup audio_drivers
 * @brief Initialize the audio codec
 * @return 0 on success, error code otherwise
 * 
 * Initializes the NAU88C22 audio codec and configures
 * all necessary registers for audio playback.
 */
uint8_t nau88c22_init(void);
```

### UI Component Functions
```
/**
 * @ingroup ui_components
 * @brief Draw a button component
 * @param x X coordinate for button position
 * @param y Y coordinate for button position
 * @param text Button text to display
 * @param pressed True if button should appear pressed
 * 
 * Draws a button at the specified coordinates with the given
 * text and visual state.
 */
void draw_button(uint16_t x, uint16_t y, const char* text, bool pressed);
```

### Data Structure with Members
```
/**
 * @brief Contact information structure
 * @ingroup data_structures
 * 
 * Stores contact information including name, phone number,
 * and metadata for the contacts database.
 */
typedef struct {
    char name[MAX_NAME_LEN];     /**< Contact's full name */
    char phone[MAX_PHONE_LEN];   /**< Phone number string */
    uint32_t id;                 /**< Unique contact identifier */
    bool favorite;               /**< True if marked as favorite */
} ContactRecord;
```

## Tips

- **Keep it simple**: Don't over-document obvious things
- **Be consistent**: Follow the same patterns throughout the codebase
- **Update when changing**: Keep documentation in sync with code changes
- **Use meaningful names**: Good function/variable names reduce need for extensive documentation
- **Group related items**: Use the same `@ingroup` for related functions

## Examples in the Codebase

Look at these well-documented files for examples:
- `include/drivers/display/display.h` - Driver documentation
- `include/kernel/data_structures/contacts_bptree.h` - Data structure documentation
- `include/ui/screen.h` - UI component documentation
- `tests/test_audio.c` - Test program documentation
