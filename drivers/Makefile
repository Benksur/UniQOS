.PHONY: all clean flash
TARGET = Firmware
BOARD ?= dev

# BOARD=pcb or BOARD=dev to define linker files
ifeq ($(BOARD), pcb)
LDSCRIPT = STM32H730XX_FLASH.ld
STARTUP_FILE = startup_stm32h730xx.s
else ifeq ($(BOARD), dev)
LDSCRIPT = STM32H753XX_FLASH.ld
STARTUP_FILE = startup_stm32h753xx.s
else
$(error Invalid BOARD specified: $(BOARD). Use 'pcb' or 'dev')
endif

TEST ?= blinky

# Specify test file (TEST=blinky, codec, )
ifeq ($(TEST), blinky)
TEST_FILE_PATH := ../../tests/test_blinky.c
else ifeq ($(TEST), codec)
TEST_FILE_PATH := ../../tests/test_nau88c22.c

else
$(error Invalid TEST specified: $(TEST))
endif

# Make sure these sources match makefile in third_party/stm32
SUBMAKE_C_SOURCES := \
$(TEST_FILE_PATH) \
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


all clean:
	$(MAKE) -C ../third_party/stm32 $@ C_SOURCES="$(SUBMAKE_C_SOURCES)" LDSCRIPT=$(LDSCRIPT) STARTUP_FILE=$(STARTUP_FILE)

flash: all
	STM32_Programmer_CLI -c port=SWD -w ../third_party/stm32/build/$(TARGET).bin 0x08000000 -v -rst
