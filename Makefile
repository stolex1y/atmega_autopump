SRC_DIR = src
OBJ_DIR = obj
BUILD_DIR = build
INCLUDE_DIR = include
RES_DIR = res
#MAKEFILE_DIR = makefile

SRCS = $(wildcard $(SRC_DIR)/*.c)
HDRS = $(wildcard $(INCLUDE_DIR)/*.h)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
#MAKEFILES := $(patsubst $(SRC_DIR)/%.c,$(MAKEFILE_DIR)/%.d,$(SRCS))
TARGET_NAME = main
TARGET = $(BUILD_DIR)/$(TARGET_NAME).hex

EEPROMDUMP = $(RES_DIR)/eeprom_dump

# Device config
DEVICE = atmega328p
CLOCK = 16000000
EEPROM_SIZE = 1024
PORT = COM7

AVRD = avrdude
AVRD_EEPROMDUMP = -U eeprom:r:$(EEPROMDUMP):r
AVRDFLAGS = -F -v -p $(DEVICE) -P $(PORT) -b57600 -c arduino -D -U flash:w:$<
CC = avr-gcc 
CFLAGS = -DEEPROM_SIZE=$(EEPROM_SIZE) -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -O2 -Wall -std=c11 -pedantic -I./$(INCLUDE_DIR) #-Werror
OBJCOPY = avr-objcopy
OBJCOPYFLAGS = -j .text -j .data -O ihex
AVRSIZEFLAGS = --format=avr --mcu=$(DEVICE)

all: $(TARGET) size

#include $(MAKEFILES)

$(TARGET): $(OBJ_DIR)/$(TARGET_NAME).elf
	@mkdir -p $(dir $@)
	$(OBJCOPY) $(OBJCOPYFLAGS) $< $@

$(OBJ_DIR)/$(TARGET_NAME).elf: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $^

flash: $(TARGET)
	$(AVRD) $(AVRDFLAGS)

dump: $(TARGET)
	$(AVRD) $(AVRDFLAGS) $(AVRD_EEPROMDUMP)
	
size: $(OBJ_DIR)/$(TARGET_NAME).elf
	avr-size $(AVRSIZEFLAGS) -C $<
	
clean:
	rm -rf $(OBJS)
	
.PHONY: clean size flash dump all