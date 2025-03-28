CC = arm-none-eabi-gcc
CFLAGS = -Wall -O2 -mcpu=cortex-m4 -mthumb
LDFLAGS = -T linker.ld

DRIVERS = driver_name_1 driver_name_2 driver_name_3

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

$(OBJ_DIR)/driver_name_1.o: $(SRC_DIR)/driver_name_1.c
$(OBJ_DIR)/driver_name_2.o: $(SRC_DIR)/driver_name_2.c
$(OBJ_DIR)/driver_name_3.o: $(SRC_DIR)/driver_name_3.c

$(OUT_FILE): $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(DRIVERS))) | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(addprefix $(OBJ_DIR)/, $(addsuffix .o, $(DRIVERS))) -o $(OUT_FILE) $(LDFLAGS)

flash: $(OUT_FILE)
	$(FLASH_TOOL) -p $(OUT_FILE)

clean:
	rm -rf $(OBJ_DIR) $(OUT_DIR)

flash_driver_1:
	$(MAKE) flash DRIVERS=driver_name_1

flash_driver_2:
	$(MAKE) flash DRIVERS=driver_name_2

flash_driver_3:
	$(MAKE) flash DRIVERS=driver_name_3

.PHONY: clean flash flash_driver_1 flash_driver_2 flash_driver_3
