CC = arm-none-eabi-gcc
CFLAGS = -Wall -O2 -mcpu=cortex-m4 -mthumb
LDFLAGS = -T linker.ld

DISPLAY_DRIVER = ili9341_lcd
AUDIO_DRIVER = wm8960_codec
MODEM_DRIVER = rc7620_modem

DRIVERS = $(DISPLAY_DRIVER) $(AUDIO_DRIVER) $(MODEM_DRIVER)

SRC_DIR = drivers
OBJ_DIR = build
OUT_DIR = $(OBJ_DIR)/out
FLASH_TOOL = stlink-cli

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OUT_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/%.bin: $(OBJ_DIR)/%.o | $(OBJ_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(OUT_DIR)/drivers.bin: $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(DRIVERS))) | $(OBJ_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

flash_lcd: $(OUT_DIR)/$(DISPLAY_DRIVER).bin
	$(FLASH_TOOL) -p $<

flash_audio: $(OUT_DIR)/$(AUDIO_DRIVER).bin
	$(FLASH_TOOL) -p $<

flash_modem: $(OUT_DIR)/$(MODEM_DRIVER).bin
	$(FLASH_TOOL) -p $<

flash: $(OUT_DIR)/drivers.bin
	$(FLASH_TOOL) -p $<

clean:
	rm -rf $(OBJ_DIR) $(OUT_DIR)

.PHONY: clean flash flash_lcd flash_audio flash_modem
