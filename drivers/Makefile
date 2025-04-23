CC = arm-none-eabi-gcc
CFLAGS = -Wall -O2 -mcpu=cortex-m4 -mthumb
LDFLAGS = -T linker.ld

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
STM32_DIR = third_party/stm32
INC_DIRS += -I$(STM32_DIR)/drivers/stm32f4xx_hal_driver/inc \
            -I$(STM32_DIR)/cmsis/device/st/stm32f4xx/include \
            -I$(STM32_DIR)/cmsis/include

CFLAGS += $(INC_DIRS)

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
TEST_DIR = tests/drivers
TEST_SRCS = $(TEST_DIR)/test_$(DISPLAY_DRIVER).c \
            $(TEST_DIR)/test_$(AUDIO_DRIVER).c \
            $(TEST_DIR)/test_$(MODEM_DRIVER).c

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OUT_DIR)

$(OBJ_DIR)/$(DISPLAY_DRIVER).o: $(SRC_DIR)/$(DISPLAY_DIR)/$(DISPLAY_DRIVER).c $(DISPLAY_HEADERS) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/$(AUDIO_DRIVER).o: $(SRC_DIR)/$(AUDIO_DIR)/$(AUDIO_DRIVER).c $(AUDIO_HEADERS) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/$(MODEM_DRIVER).o: $(SRC_DIR)/$(MODEM_DIR)/$(MODEM_DRIVER).c $(MODEM_HEADERS) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

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
$(OBJ_DIR)/test_%.o: $(TEST_DIR)/test_%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Modified flash targets
flash_lcd: $(OUT_DIR)/test_$(DISPLAY_DRIVER).bin
	$(FLASH_TOOL) -p $<

flash_audio: $(OUT_DIR)/test_$(AUDIO_DRIVER).bin
	$(FLASH_TOOL) -p $<

flash_modem: $(OUT_DIR)/test_$(MODEM_DRIVER).bin
	$(FLASH_TOOL) -p $<

flash: $(OUT_DIR)/drivers.bin
	$(FLASH_TOOL) -p $<

clean:
	rm -rf $(OBJ_DIR) $(OUT_DIR)

.PHONY: clean flash flash_lcd flash_audio flash_modem
