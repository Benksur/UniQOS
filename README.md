# UniQOS

A FreeRTOS based operating system for a custom embedded mobile phone inspired by the Nokia 2730c.

Project under the supervision of Professor Pauline Pounds.

UQ Phone Team Year 1 (2025):
Ty Behnke (Firmware)
James Wood (Hardware)
Caiyan Jin (UI/UX Design)

## Dependencies

- **STM32_Programmer_CLI** - STM32 programming tool
- **arm-none-eabi-gcc** (version 14.2) - ARM cross-compiler
- **Doxygen** - Documentation generation

Make sure these tools are installed and accessible in your PATH variables.

## Build Instructions

### Running Tests
```bash
make BOARD=(dev or pcb) TEST=(display, modem etc..)
```
*Optional: add `-j(cpu thread-count)` flag to speed up execution*

### Main OS
```bash
make BOARD=(dev or pcb)
```

## Documentation Generation

To generate the project documentation:

```bash
doxygen Doxyfile
```

The documentation will be generated in the `docs/html/` directory. Open `docs/html/index.html` in your web browser to view the complete API documentation.


