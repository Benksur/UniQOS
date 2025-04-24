# Simple forwarding Makefile for STM32 using sub-make
.PHONY: all clean flash

# Default TARGET from the STM32 Makefile
TARGET = Firmware

# --- Define the C_SOURCES list for the sub-make --- 
# Paths must be relative to the sub-make directory (../third_party/stm32)
# test_blinky.c path: ../../tests/test_blinky.c
# Exclude the original main.c (Core/Src/main.c)
# This list needs to be kept in sync with the third_party/stm32/Makefile!
SUBMAKE_C_SOURCES = \
../../tests/test_blinky.c \
Core/Src/gpio.c \
Core/Src/dma.c \
Core/Src/fmc.c \
Core/Src/i2c.c \
Core/Src/i2s.c \
Core/Src/memorymap.c \
Core/Src/rtc.c \
Core/Src/sdmmc.c \
Core/Src/tim.c \
Core/Src/usart.c \
Core/Src/stm32h7xx_it.c \
Core/Src/stm32h7xx_hal_msp.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_cortex.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc_ex.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_flash.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_flash_ex.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_gpio.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_hsem.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma_ex.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_mdma.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr_ex.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2c.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2c_ex.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_exti.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_fmc.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_nor.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_sram.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_nand.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_sdram.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2s.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2s_ex.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rtc.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rtc_ex.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_sdmmc.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_delayblock.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_sd.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_sd_ex.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_mmc.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_mmc_ex.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim_ex.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart_ex.c \
Core/Src/system_stm32h7xx.c

# Forward standard targets to STM32 Makefile, overriding C_SOURCES
all clean:
	$(MAKE) -C ../third_party/stm32 $@ C_SOURCES="$(SUBMAKE_C_SOURCES)"

# Flash command using the output from the STM32 build directory
# Depends on 'all' to ensure the build is complete first
flash: all
	STM32_Programmer_CLI -c port=SWD -w ../third_party/stm32/build/$(TARGET).bin 0x08000000 -v -rst
