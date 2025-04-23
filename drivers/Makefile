CC = arm-none-eabi-gcc
# Simplify: remove type redefinitions but keep including stdint.h
CFLAGS = -Wall -O2 -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard -include stdint.h
LDFLAGS = -T../third_party/stm32/linker.ld -specs=nano.specs -lc -lm -lnosys -Wl,--gc-sections

BOARD_DIR = board
DISPLAY_DIR = display
AUDIO_DIR = audio
MODEM_DIR = modem

INC_DIR = include/drivers
INC_DIRS = -Iinclude/drivers \
           -Iinclude/drivers/$(DISPLAY_DIR) \
           -Iinclude/drivers/$(AUDIO_DIR) \
           -Iinclude/drivers/$(MODEM_DIR) \
           -Iinclude/board

# Add STM32 HAL include path
STM32_DIR = ../third_party/stm32
INC_DIRS += -I$(STM32_DIR)/Core/Inc \
            -I$(STM32_DIR)/Drivers/CMSIS/Include \
            -I$(STM32_DIR)/Drivers/CMSIS/Device/ST/STM32H7xx/Include \
            -I$(STM32_DIR)/Drivers/STM32H7xx_HAL_Driver/Inc \
            -I$(STM32_DIR)/Drivers/CMSIS/Core/Include \
            -isystem /usr/arm-none-eabi/include

# Add STM32H7 defines to the CFLAGS
FULL_CFLAGS = $(CFLAGS) $(INC_DIRS) -DSTM32H743xx -DUSE_HAL_DRIVER


# STM32 HAL sources
STM32_SRCS = $(STM32_DIR)/Core/Src/system_stm32h7xx.c \
             $(STM32_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal.c \
             $(STM32_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_cortex.c \
             $(STM32_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_gpio.c \
             $(STM32_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc.c \
             $(STM32_DIR)/Core/Src/main.c \
             $(STM32_DIR)/Core/Src/stm32h7xx_hal_msp.c \
             $(STM32_DIR)/Core/Src/stm32h7xx_it.c \
             $(STM32_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rcc_ex.c \
             $(STM32_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma.c \
             $(STM32_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_dma_ex.c \
             $(STM32_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim.c \
             $(STM32_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_tim_ex.c \
             $(STM32_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_sd.c \
             $(STM32_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2c.c \
             $(STM32_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2c_ex.c \
             $(STM32_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_i2s.c \
             $(STM32_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_sram.c \
             $(STM32_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart.c \
             $(STM32_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_uart_ex.c \
             $(STM32_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr.c \
             $(STM32_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_pwr_ex.c \
             $(STM32_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_sdmmc.c \
             $(STM32_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_ll_fmc.c \
             $(STM32_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_mdma.c \
             $(STM32_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rtc.c \
             $(STM32_DIR)/Drivers/STM32H7xx_HAL_Driver/Src/stm32h7xx_hal_rtc_ex.c

# Startup file
STARTUP_SRC = $(STM32_DIR)/Drivers/CMSIS/Device/ST/STM32H7xx/Source/Templates/gcc/startup_stm32h743xx.s

# Get object files for STM32 sources
STM32_OBJS = $(patsubst $(STM32_DIR)/%.c,$(OBJ_DIR)/stm32_%.o,$(STM32_SRCS))
STARTUP_OBJ = $(OBJ_DIR)/startup.o

DISPLAY_DRIVER = ili9341_lcd
AUDIO_DRIVER = nau88c22
MODEM_DRIVER = rc7620

DRIVERS = $(DISPLAY_DRIVER) $(AUDIO_DRIVER) $(MODEM_DRIVER)

SRC_DIR = drivers
OBJ_DIR = build
OUT_DIR = $(OBJ_DIR)/out
FLASH_TOOL = stlink-cli

# Header dependencies
DISPLAY_HEADERS = $(INC_DIR)/$(DISPLAY_DIR)/$(DISPLAY_DRIVER).h
AUDIO_HEADERS = $(INC_DIR)/$(AUDIO_DIR)/$(AUDIO_DRIVER).h
MODEM_HEADERS = $(INC_DIR)/$(MODEM_DIR)/$(MODEM_DRIVER).h

# Test files directory
TEST_DIR = ../tests
TESTS_DRIVERS_DIR = $(TEST_DIR)/drivers
DRIVER_TEST_SRCS = $(TESTS_DRIVERS_DIR)/test_$(DISPLAY_DRIVER).c \
            $(TESTS_DRIVERS_DIR)/test_$(AUDIO_DRIVER).c \
            $(TESTS_DRIVERS_DIR)/test_$(MODEM_DRIVER).c

# Blinky test
BLINKY_SRC = $(TEST_DIR)/test_blinky.c
BLINKY_OBJ = $(OBJ_DIR)/test_blinky.o
BLINKY_BIN = $(OUT_DIR)/test_blinky.bin

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OUT_DIR)

$(OBJ_DIR)/$(DISPLAY_DRIVER).o: $(SRC_DIR)/$(DISPLAY_DIR)/$(DISPLAY_DRIVER).c $(DISPLAY_HEADERS) | $(OBJ_DIR)
	$(CC) $(FULL_CFLAGS) -c $< -o $@

$(OBJ_DIR)/$(AUDIO_DRIVER).o: $(SRC_DIR)/$(AUDIO_DIR)/$(AUDIO_DRIVER).c $(AUDIO_HEADERS) | $(OBJ_DIR)
	$(CC) $(FULL_CFLAGS) -c $< -o $@

$(OBJ_DIR)/$(MODEM_DRIVER).o: $(SRC_DIR)/$(MODEM_DIR)/$(MODEM_DRIVER).c $(MODEM_HEADERS) | $(OBJ_DIR)
	$(CC) $(FULL_CFLAGS) -c $< -o $@

$(BLINKY_OBJ): $(BLINKY_SRC) | $(OBJ_DIR)
	$(CC) $(FULL_CFLAGS) -c $< -o $@

# Rules for STM32 sources
$(OBJ_DIR)/stm32_%.o: $(STM32_DIR)/%.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(FULL_CFLAGS) -c $< -o $@

# Rule for startup file
$(STARTUP_OBJ): $(STARTUP_SRC) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Define the main.o path explicitly for clarity
MAIN_OBJ = $(OBJ_DIR)/stm32_Core/Src/main.o

$(BLINKY_BIN): $(BLINKY_OBJ) $(STM32_OBJS) $(STARTUP_OBJ) | $(OBJ_DIR)
	$(CC) -o $@ $(BLINKY_OBJ) $(STM32_OBJS) $(STARTUP_OBJ) $(FULL_CFLAGS) $(LDFLAGS) -Wl,-Map=$(OUT_DIR)/test_blinky.map

$(OUT_DIR)/%.bin: $(OBJ_DIR)/%.o | $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(OUT_DIR)/drivers.bin: $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(DRIVERS))) | $(OBJ_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Modified object file rules to include test files
$(OUT_DIR)/test_$(DISPLAY_DRIVER).bin: $(OBJ_DIR)/$(DISPLAY_DRIVER).o $(OBJ_DIR)/test_$(DISPLAY_DRIVER).o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OUT_DIR)/test_$(AUDIO_DRIVER).bin: $(OBJ_DIR)/$(AUDIO_DRIVER).o $(OBJ_DIR)/test_$(AUDIO_DRIVER).o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OUT_DIR)/test_$(MODEM_DRIVER).bin: $(OBJ_DIR)/$(MODEM_DRIVER).o $(OBJ_DIR)/test_$(MODEM_DRIVER).o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Test object files
$(OBJ_DIR)/test_%.o: $(TESTS_DRIVERS_DIR)/test_%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Modified flash targets
flash_lcd: $(OUT_DIR)/test_$(DISPLAY_DRIVER).bin
	$(FLASH_TOOL) -p $<

flash_audio: $(OUT_DIR)/test_$(AUDIO_DRIVER).bin
	$(FLASH_TOOL) -p $<

flash_modem: $(OUT_DIR)/test_$(MODEM_DRIVER).bin
	$(FLASH_TOOL) -p $<

flash_blinky: $(BLINKY_BIN)
	$(FLASH_TOOL) -p $<

flash: $(OUT_DIR)/drivers.bin
	$(FLASH_TOOL) -p $<

blinky: $(BLINKY_BIN)

clean:
	rm -rf $(OBJ_DIR) $(OUT_DIR)

.PHONY: clean flash flash_lcd flash_audio flash_modem flash_blinky blinky
