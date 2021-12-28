#include <avr/io.h>

#define HIGH 0x1
#define LOW 0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

void bit_clear(uint8_t port, uint8_t bit) {
	port &= ~(1 << bit);
}

void bit_set(uint8_t port, uint8_t bit) {
	port |= 1 << bit;
}

void bit_write(uint8_t port, uint8_t bit, bool val) {
	val ? bit_set(port, bit) : bit_clear(port, bit);
}

void pin_mode(uint8_t pin, uint8_t mode) {
	switch (mode) {
		case INPUT:
			if (pin < 8) {
				bit_clear(DDRD, pin);
				bit_clear(PORTD, pin);
			} else if (pin < 14) {
				bit_clear(DDRB, pin - 8);
				bit_clear(PORTB, pin - 8);
			} else if (pin < 20) {
				bit_clear(DDRC, pin - 14);
				bit_clear(PORTC, pin - 14);
			}
			return;
			
		case OUTPUT:
			if (pin < 8) {
				bit_set(DDRD, pin);
				bit_clear(PORTD, pin);
			} else if (pin < 14) {
				bit_set(DDRB, pin - 8);
				bit_clear(PORTB, pin - 8);
			} else if (pin < 20) {
				bit_set(DDRC, pin - 14);
				bit_clear(PORTC, pin - 14);
			}
			return;
		
		case INPUT_PULLUP:
			if (pin < 8) {
				bit_clear(DDRD, pin);
				bit_set(PORTD, pin);
			} else if (pin < 14) {
				bit_clear(DDRB, pin - 8);
				bit_set(PORTB, pin - 8);
			} else if (pin < 20) {
				bit_clear(DDRC, pin - 14);
				bit_set(PORTC, pin - 14);
			}
			return;
	}
}

void digital_write(uint8_t pin, bool val) {
	// TODO отключение шим
	
	
}