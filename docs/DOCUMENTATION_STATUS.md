# Documentation Status

## Summary

The UniQOS codebase is now organized with a hierarchical Doxygen group structure defined in `docs/groups.dox`. Individual header files use `@ingroup` to place themselves within this structure.

## Documentation Guides

- **`docs/groups.dox`** - Central module group definitions
- **`docs/STRUCT_DOCUMENTATION_GUIDE.md`** - How to document structs, enums, and classes
- **`docs/DOCUMENTATION_STATUS.md`** - This file, tracking progress

## Completed Documentation

### Driver Headers
- ✅ `include/drivers/audio/nau88c22.h` - NAU88C22 audio codec
- ✅ `include/drivers/audio/iaudio_driver.h` - Audio driver interface
- ✅ `include/drivers/display/display.h` - Display driver (fully documented)
- ✅ `include/drivers/display/st7789v.h` - ST7789V LCD controller
- ✅ `include/drivers/display/idisplay_driver.h` - Display driver interface
- ✅ `include/drivers/modem/modem.h` - Modem interface
- ✅ `include/drivers/modem/at_commands.h` - AT command interface
- ✅ `include/drivers/peripherals/keypad.h` - Keypad driver
- ✅ `include/drivers/peripherals/lsm6dsv.h` - IMU sensor (partial)
- ✅ `include/drivers/peripherals/drv2603.h` - Haptic motor driver
- ✅ `include/drivers/power/bq27441.h` - Battery fuel gauge

### Kernel Headers
- ✅ `include/kernel/kernel.h` - Kernel core
- ✅ `include/kernel/tasks/display_task.h` - Display task
- ✅ `include/kernel/tasks/call_state.h` - Call state machine
- ✅ `include/kernel/tasks/cellular_task.h` - Cellular task
- ✅ `include/kernel/tasks/audio_task.h` - Audio task
- ✅ `include/kernel/tasks/power_task.h` - Power task
- ✅ `include/kernel/data_structures/contacts_bptree.h` - B+ tree
- ✅ `include/kernel/sms_types.h` - SMS data structures

### UI Headers
- ✅ `include/ui/screen.h` - Screen management
- ✅ `include/ui/theme.h` - UI theming
- ✅ `include/ui/input.h` - Input handling
- ✅ `include/ui/multitap.h` - Multitap text entry

### Configuration Files
- ✅ `docs/groups.dox` - Central group definitions
- ✅ `docs/mainpage.dox` - Main documentation page

## Documentation Structure

```
docs/
├── groups.dox          # Central module group definitions
├── mainpage.dox        # Main documentation page
└── DOCUMENTATION_STATUS.md  # This file

Modules Hierarchy:
├── drivers/            # Hardware drivers
│   ├── audio_drivers/
│   │   └── nau88c22_driver
│   ├── display_drivers/
│   │   ├── st7789v_driver
│   │   └── display_driver
│   ├── modem_drivers/
│   │   ├── modem_driver
│   │   ├── at_commands
│   │   └── rc7620_api
│   ├── peripheral_drivers/
│   │   ├── keypad_driver
│   │   ├── imu_driver
│   │   ├── haptic_driver
│   │   └── led_driver
│   └── power_drivers/
│       ├── bq27441_driver
│       └── mcp73871_driver
├── kernel/
│   ├── kernel_core
│   ├── tasks/
│   │   ├── display_task
│   │   ├── cellular_task
│   │   ├── call_state_task
│   │   ├── audio_task
│   │   └── power_task
│   ├── data_structures/
│   │   ├── contacts_bptree
│   │   └── sms_types
│   └── memory
├── ui/
│   ├── ui_core/
│   │   ├── ui_screen
│   │   └── ui_theme
│   ├── ui_components
│   ├── ui_pages
│   ├── ui_overlays
│   └── ui_input
└── audio/
    ├── audio_mixer
    └── audio_oscillator
```

## How to Document New Files

### 1. File Header
Add this at the top of every header file:

```c
/**
 * @file filename.h
 * @brief Brief description of the file
 * @ingroup group_name
 * 
 * Detailed description of what this module does.
 */
```

### 2. Functions
Document each function:

```c
/**
 * @ingroup group_name
 * @brief Brief function description
 * @param param1 Description of param1
 * @param param2 Description of param2
 * @return Description of return value
 */
returntype function_name(type1 param1, type2 param2);
```

### 3. Data Structures
Document structs and their members:

```c
/**
 * @brief Structure description
 * @ingroup group_name
 */
typedef struct {
    int field1;      /**< Description of field1 */
    char *field2;    /**< Description of field2 */
} my_struct_t;
```

### 4. Enumerations
Document enums and their values:

```c
/**
 * @brief Enum description
 * @ingroup group_name
 */
typedef enum {
    VALUE_ONE = 0,    /**< Description of VALUE_ONE */
    VALUE_TWO,        /**< Description of VALUE_TWO */
    VALUE_THREE       /**< Description of VALUE_THREE */
} my_enum_t;
```

### 5. Constants and Defines
Document macros:

```c
/** @ingroup group_name
 *  @brief Description of this constant */
#define MY_CONSTANT 42
```

## Remaining Files to Document

### High Priority
- ✅ **ALL HIGH PRIORITY FILES COMPLETED!** 🎉

### Medium Priority
- ✅ `include/audio/mixer.h` - Audio mixer
- ✅ `include/audio/oscillator.h` - Audio oscillator
- ✅ `include/ui/theme.h` - UI theming
- ✅ `include/ui/input.h` - Input handling
- ✅ `include/ui/multitap.h` - Multitap text entry

### Lower Priority (UI Components and Pages)
- [ ] All files in `include/ui/components/`
- [ ] All files in `include/ui/pages/`
- [ ] All files in `include/ui/overlays/`

## Building Documentation

To generate the Doxygen documentation:

```bash
cd /home/benksur/Documents/uni/reit4841/UniQOS
doxygen Doxyfile
```

The output will be in:
- HTML: `docs/html/index.html`
- LaTeX: `docs/latex/`

## Best Practices

1. **Always use `@ingroup`** to place items in the correct module group
2. **Document all public APIs** - functions, structs, enums, and macros
3. **Use `@brief` for short descriptions** - keep it to one sentence
4. **Add detailed descriptions** when needed for complex functions
5. **Document parameters and return values** - be specific about units, ranges, and special values
6. **Use inline comments** (`/**< comment */`) for struct/enum members
7. **Keep documentation up to date** when modifying code

## Tips for Good Documentation

- **Be concise but complete** - explain what, not how
- **Include units** - "timeout in milliseconds", "voltage in mV"
- **Note special cases** - "returns NULL if allocation fails"
- **Cross-reference** - use `@ref` to link to related functions
- **Add examples** - use `@code` blocks for complex usage
- **Mark deprecated items** - use `@deprecated` tag

## Next Steps

1. Continue documenting high-priority files listed above
2. Add more detailed descriptions to complex functions
3. Consider adding usage examples to key modules
4. Review generated documentation and fix any warnings
5. Keep documentation synchronized with code changes

