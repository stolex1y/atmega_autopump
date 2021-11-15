DEVICE = atmega328p
CLOCK = 16000000
PORT = COM7
AVRD = avrdude
AVRDFLAGS = -F -v -p $(DEVICE) -P $(PORT) -b57600 -c arduino -D
CC = avr-gcc 
CFLAGS = -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -O2 -Wall -Werror

OBJCOPY = avr-objcopy
SRC = src
BIN = bin
TARGET = main
SOURCES = blink_test.c


all: $(BIN)/$(TARGET).hex size

$(BIN)/$(TARGET).o: $(SRC)/$(SOURCES)
	$(CC) $(CFLAGS) $^ -o $@ -c

$(BIN)/$(TARGET).elf: $(BIN)/$(TARGET).o
	$(CC) $(CFLAGS) $^ -o $@
	
$(BIN)/$(TARGET).hex: $(BIN)/$(TARGET).elf	
	$(OBJCOPY) -j .text -j .data -O ihex $^ $@

flash: $(BIN)/$(TARGET).hex
	$(AVRD) $(AVRDFLAGS) -U flash:w:$<
	
size: $(BIN)/$(TARGET).elf
	avr-size -C $< --format=avr --mcu=$(DEVICE)
	
clean:
	-rm -rf $(BIN)\$(TARGET).hex $(BIN)\$(TARGET).o $(BIN)\$(TARGET).elf
	
.PHONY: clean size flash all