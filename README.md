<div align="center">

# UniQOS

<img src="https://img.shields.io/badge/License-GPL-informational?style=for-the-badge&logo=github&logoColor=white&color=blue" alt="License Badge" />

<img src="https://img.shields.io/badge/Platform-STM32H7-informational?style=for-the-badge&logo=stmicroelectronics&logoColor=white&color=orange" alt="Platform Badge" />

<img src="https://img.shields.io/badge/RTOS-FreeRTOS-informational?style=for-the-badge&logo=freertos&logoColor=white&color=green" alt="FreeRTOS Badge" />


---

</div>

## About

UniQOS is a FreeRTOS based operating system for a custom embedded mobile phone inspired by the Nokia 2730c. This project combines modern embedded systems design with the simplicity and reliability of classic mobile phones, creating a fully-functional communication device from the ground up.


**Supervised by:** Professor Pauline Pounds  
**Institution:** University of Queensland
<div align="center">

### UQ Phone Team (Year 1 - 2025)

| Name | Role |
|------|------|
| **Ty Behnke** | Firmware Design |
| **James Wood** | Hardware Design |
| **Caiyan Jin** | UI/UX Design |
</div>

---



## Dependencies

Make sure the following tools are installed and accessible in your `PATH`:
<div align="center">

| Tool | Version | Purpose |
|------|---------|---------|
| **STM32_Programmer_CLI** | Latest | STM32 programming tool |
| **arm-none-eabi-gcc** | 14.2+ | ARM cross-compiler toolchain |
| **Doxygen** | Latest | API documentation generation |
</div>


---

## Build Instructions

The `BOARD` flag will specify the chip target, for building on the existing board use `pcb`. For testing on personal development boards using the H7 chipsets, you may need to modify the `/driver` and `/kernel` Makefiles depending on the exact chipset used by the board.

### Running Tests
```bash
make BOARD=(dev or pcb) TEST=(display, modem etc..)
```


### Main OS
```bash
make BOARD=(dev or pcb)
```

*Optional: add `-j(cpu thread-count)` flag to speed up execution*

---

## Documentation

To generate the project documentation:

```bash
doxygen Doxyfile
```

The documentation will be generated in the `docs/html/` directory. Open `docs/html/index.html` in your web browser to view the complete API documentation.


---

## Contact

Reach out if you have any questions: [tyjbehnke@gmail.com](mailto:tyjbehnke@gmail.com)
