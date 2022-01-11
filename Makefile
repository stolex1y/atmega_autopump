DEVICE = atmega328p
CLOCK = 16000000
EEPROM_SIZE = 1024
PORT = COM7

AVRD = avrdude
AVRDFLAGS = -F -v -p $(DEVICE) -P $(PORT) -b57600 -c arduino -D -U flash:w:$< #-U eeprom:r:eeprom_dump:r

CC = avr-gcc 
CFLAGS = -DEEPROM_SIZE=1024 -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -O2 -Wall -std=c11 #-Werror

OBJCOPY = avr-objcopy
COPYFLAGS = -j .text -j .data -O ihex

AVRSFLAGS = --format=avr --mcu=$(DEVICE)

SRC = src
BIN = bin
TARGET = main
#SOURCES = blink_test.c gpio.c
OBJECTS = $(BIN)/blink_test.o $(BIN)/gpio.o $(BIN)/timer.o $(BIN)/lcd1602.o $(BIN)/eeprom.o $(BIN)/coordinates.o $(BIN)/encoder.o

VPATH += $(BIN) $(SRC)

all: $(BIN)/$(TARGET).hex size

$(BIN)/%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@ -c

$(BIN)/$(TARGET).elf: $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@
	
$(BIN)/$(TARGET).hex: $(BIN)/$(TARGET).elf
	$(OBJCOPY) $(COPYFLAGS) $^ $@

flash: $(BIN)/$(TARGET).hex
	$(AVRD) $(AVRDFLAGS)
	
size: $(BIN)/$(TARGET).elf
	avr-size $(AVRSFLAGS) -C $<
	
clean:
	-rm -rf $(BIN)
	mkdir $(BIN)
	
.PHONY: clean size flash all