#include "gpio.h"

#include <avr/io.h>

#include <stdbool.h>
#include <stdint.h>

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

        default:
            break;
	}
}

void digital_write(uint8_t pin, bool val) {
    if (pin < 8) {
        bit_write(PORTD, pin, val);
    } else if (pin < 14) {
        bit_write(PORTB, (pin - 8), val);
    } else if (pin < 20) {
        bit_write(PORTC, (pin - 14), val);
    }
	
}

bool digital_read(uint8_t pin) {
    if (pin < 8) {
        return bit_read(PIND, pin);
    } else if (pin < 14) {
        return bit_read(PINB, pin - 8);
    } else if (pin < 20) {
        return bit_read(PINC, pin - 14);
    }
    return 0;
}

void digital_toggle(uint8_t pin) {
    if (pin < 8) {
        bit_toggle(PORTD, pin);
    } else if (pin < 14) {
        bit_toggle(PORTB, (pin - 8));
    } else if (pin < 20) {
        bit_toggle(PORTC, (pin - 14));
    }
}

bool analog_read(uint8_t pin) {
    //TODO analog_read
    return 0;
}

void analog_write(uint8_t pin, bool val) {
    //TODO analog_write
}






