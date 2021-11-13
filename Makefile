DEVICE = atmega328p
CLOCK = 16000000
PORT = COM7
AVRD = C:\Arduino\hardware\tools\avr/bin/avrdude
AVRDFLAGS = -C C:\Arduino\hardware\tools\avr/etc/avrdude.conf -v -p $(DEVICE) -c arduino -P $(PORT) -b57600 -D 
CC = avr-gcc 
CFLAGS = -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -O2 -Wall -Werror

OBJCOPY = avr-objcopy
SRC = src
BIN = bin
TARGET = main
SOURCES = blink_test.c


all: $(BIN)/$(TARGET).hex size

$(BIN)/$(TARGET).bin: $(SRC)/$(SOURCES)
	$(CC) -c $^ -o $@ $(CFLAGS) 
	
$(BIN)/$(TARGET).hex: $(BIN)/$(TARGET).bin
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

flash: $(BIN)/$(TARGET).hex
	$(AVRD) $(AVRDFLAGS) -U flash:w:$<:a
	
size: $(BIN)/$(TARGET).bin
	avr-size -C $< --format=avr --mcu=$(DEVICE)
	
clean:
	-rm -rf $(BIN)\$(TARGET).hex $(BIN)\$(TARGET).bin
	
.PHONY: clean size flash all