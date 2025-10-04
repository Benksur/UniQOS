# UniQOS

Custom embedded operating system for mobile devices built on STM32 ecosystem.

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

## Project Information

For detailed project information, architecture overview, hardware components, and development guidelines, see the [project documentation](docs/html/index.html) or the [mainpage.dox](docs/mainpage.dox) file.


