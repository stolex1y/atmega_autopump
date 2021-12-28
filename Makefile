DEVICE = atmega328p
CLOCK = 16000000
PORT = COM7

AVRD = avrdude
AVRDFLAGS = -F -v -p $(DEVICE) -P $(PORT) -b57600 -c arduino -D -U flash:w:$<

CC = avr-gcc 
CFLAGS = -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -O2 -Wall -Werror

OBJCOPY = avr-objcopy
COPYFLAGS = -j .text -j .data -O ihex

AVRSFLAGS = --format=avr --mcu=$(DEVICE)

SRC = src
BIN = bin
TARGET = main
#SOURCES = blink_test.c gpio.c
OBJECTS = $(BIN)/blink_test.o $(BIN)/gpio.o

VPATH += $(BIN) $(SRC)

all: $(BIN)/$(TARGET).hex size

build:
	mkdir bin

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
	-rm -rf $(BIN)\$(TARGET).hex $(BIN)\$(TARGET).o $(BIN)\$(TARGET).elf
	
.PHONY: clean size flash all