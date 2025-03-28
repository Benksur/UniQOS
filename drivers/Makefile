CC = arm-none-eabi-gcc
CFLAGS = -Wall -O2 -mcpu=cortex-m4 -mthumb
LDFLAGS = -T linker.ld

DRIVER1 = driver_name_1
DRIVER2 = driver_name_2
DRIVER3 = driver_name_3

DRIVERS = $(DRIVER1) $(DRIVER2) $(DRIVER3)

SRC_DIR = drivers
OBJ_DIR = build
OUT_DIR = $(OBJ_DIR)/out
FLASH_TOOL = stlink-cli

OUT_FILE = $(OUT_DIR)/drivers.bin

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OUT_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/$(DRIVER1).o: $(SRC_DIR)/$(DRIVER1).c
$(OBJ_DIR)/$(DRIVER2).o: $(SRC_DIR)/$(DRIVER2).c
$(OBJ_DIR)/$(DRIVER3).o: $(SRC_DIR)/$(DRIVER3).c

$(OUT_FILE): $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(DRIVERS))) | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(DRIVERS))) -o $(OUT_FILE) $(LDFLAGS)

flash: $(OUT_FILE)
	$(FLASH_TOOL) -p $(OUT_FILE)

clean:
	rm -rf $(OBJ_DIR) $(OUT_DIR)

flash_$(DRIVER1):
	$(MAKE) flash DRIVERS=$(DRIVER1)

flash_$(DRIVER2):
	$(MAKE) flash DRIVERS=$(DRIVER2)

flash_$(DRIVER3):
	$(MAKE) flash DRIVERS=$(DRIVER3)

.PHONY: clean flash flash_$(DRIVER1) flash_$(DRIVER2) flash_$(DRIVER3)
