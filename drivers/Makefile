.PHONY: all clean flash
TARGET = Firmware
BOARD ?= dev

# Define linker and startup file based on BOARD
ifeq ($(BOARD), pcb)
LDSCRIPT = STM32H733XG_FLASH.ld
STARTUP_FILE = startup_stm32h733xx.s
BOARD_C_DEF = -DSTM32H733xx
else ifeq ($(BOARD), dev)
LDSCRIPT = STM32H753XX_FLASH.ld
STARTUP_FILE = startup_stm32h753xx.s
BOARD_C_DEF = -DSTM32H753xx \
              -DRCC_USART16910CLKSOURCE_D2PCLK2=RCC_USART16CLKSOURCE_D2PCLK2 \
              -DRCC_USART16910CLKSOURCE_PLL2=RCC_USART16CLKSOURCE_PLL2 \
              -DRCC_USART16910CLKSOURCE_PLL3=RCC_USART16CLKSOURCE_PLL3 \
              -DRCC_USART16910CLKSOURCE_HSI=RCC_USART16CLKSOURCE_HSI \
              -DRCC_USART16910CLKSOURCE_CSI=RCC_USART16CLKSOURCE_CSI \
              -DRCC_USART16910CLKSOURCE_LSE=RCC_USART16CLKSOURCE_LSE
else
$(error Invalid BOARD specified: $(BOARD). Use 'pcb' or 'dev')
endif

# Combine common and board-specific C definitions
# These will override the C_DEFS in the sub-makefile
COMMON_C_DEFS = -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DDEBUG
SUBMAKE_C_DEFS := $(COMMON_C_DEFS) $(BOARD_C_DEF) \
				  -I../../include/board \
                  -I../../include/drivers \
                  -I../../include/drivers/audio \
                  -I../../include/drivers/power \
                  -I../../include/drivers/modem \
                  -I../../include/drivers/display \
                  -I../../include/drivers/peripherals \
                  -I../../include/kernel \

TEST ?=

# Specify test file (TEST=blinky, codec, )
ifeq ($(TEST), )
# Ignore empty - no test
else ifeq ($(TEST), blinky)
TEST_FILE_PATH := ../../tests/test_blinky.c
TARGET := test_blinky
else ifeq ($(TEST), codec)
TEST_FILE_PATH := ../../tests/test_nau88c22.c
TARGET := test_nau88c22
else ifeq ($(TEST), mobile)
TEST_FILE_PATH := ../../tests/test_rc7620.c
TARGET := test_rc7620
else ifeq ($(TEST), terminal)
TEST_FILE_PATH := ../../tests/test_terminal.c
TARGET := test_terminal
else ifeq ($(TEST), display)
TEST_FILE_PATH := ../../tests/test_st7789v.c
TARGET := test_display
else ifeq ($(TEST), microsd)
TEST_FILE_PATH := ../../tests/test_microsd.c
TARGET := test_microsd
else ifeq ($(TEST), keypad)
TEST_FILE_PATH := ../../tests/test_keypad.c
TARGET := test_keypad
else ifeq ($(TEST), imu)
TEST_FILE_PATH := ../../tests/test_imu.c
TARGET := test_imu
else ifeq ($(TEST), vib)
TEST_FILE_PATH := ../../tests/test_vib.c
TARGET := test_vib
else ifeq ($(TEST), leds)
TEST_FILE_PATH := ../../tests/test_leds.c
TARGET := test_leds
else ifeq ($(TEST), batt)
TEST_FILE_PATH := ../../tests/test_batt.c
TARGET := test_batt
else
$(error Invalid TEST specified: $(TEST))
endif

# Custom driver sources - files specific to project
CUSTOM_DRIVER_SOURCES := \
$(TEST_FILE_PATH) \
$(CSRCS) \
../../drivers/display/st7789v.c \
../../drivers/display/LCD_Controller.c \
../../drivers/power/bq27441.c \
../../drivers/power/mcp73871.c \
../../drivers/audio/nau88c22.c \
../../drivers/modem/modem_terminal.c \
../../drivers/modem/modem.c \
../../drivers/modem/at_commands.c \
../../drivers/modem/rc7620_api.c \
../../drivers/peripherals/lsm6dsv.c \
../../drivers/peripherals/drv2603.c \
../../drivers/peripherals/ws2812.c \


# copy and pasted from third_party/stm32 makefile. this NEEDS to be redefined because 
# we are overwriting the C_SOURCES in the generated stm makefile
STM32_BASE_SOURCES := \
Core/Src/gpio.c \
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
Core/Src/system_stm32h7xx.c \
Core/Src/spi.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_spi.c \
Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_spi_ex.c \
FATFS/App/fatfs.c \
FATFS/Target/bsp_driver_sd.c \
FATFS/Target/sd_diskio.c \
FATFS/Target/fatfs_platform.c \
Middlewares/Third_Party/FatFs/src/diskio.c \
Middlewares/Third_Party/FatFs/src/ff.c \
Middlewares/Third_Party/FatFs/src/ff_gen_drv.c \
Middlewares/Third_Party/FatFs/src/option/syscall.c \
Middlewares/Third_Party/FatFs/src/option/ccsbcs.c

# Combined sources for passing to third_party makefile
ALL_C_SOURCES := $(STM32_BASE_SOURCES) $(CUSTOM_DRIVER_SOURCES)
# $(info AFLAGS=$(AFLAGS))

all clean:
	$(MAKE) -C ../third_party/stm32 $@ TARGET="$(TARGET)" C_SOURCES="$(ALL_C_SOURCES)" LDSCRIPT=$(LDSCRIPT) ASM_SOURCES=$(STARTUP_FILE) C_DEFS="$(SUBMAKE_C_DEFS)" BUILD_DIR="../../build" 

flash: all
	STM32_Programmer_CLI -c port=SWD -w ../build/$(TARGET).bin 0x08000000 -v -rst