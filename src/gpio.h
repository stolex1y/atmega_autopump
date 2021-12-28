#ifndef ARDUINO_AUTOPUMP_GPIO_H
#define ARDUINO_AUTOPUMP_GPIO_H

#include <stdbool.h>
#include <stdint.h>

#define HIGH 0x1
#define LOW 0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

#define bit_clear(reg, bit) (reg &= ~(1 << (bit)))
#define bit_set(reg, bit)	(reg |= 1 << (bit))
#define bit_write(reg, bit, val) (val ? bit_set(reg, bit) : bit_clear(reg, bit))
#define bit_read(reg, bit) ((reg >> (bit)) & 1)
#define bit_toggle(reg, bit) (bit_write(reg, bit, ~bit_read(reg, bit) & 1))

#define high_byte(val) ((val >> 8) & 255)
#define low_byte(val) (val & 255)

#define reg16_write(regH, regL, val) regH = high_byte(val); regL = low_byte(val)
#define reg16_read(regH, regL) (regL | (regH << 8))

void pin_mode(uint8_t pin, uint8_t mode);
void digital_write(uint8_t pin, bool val);
bool digital_read(uint8_t pin);
void digital_toggle(uint8_t pin);
bool analog_read(uint8_t pin);
void analog_write(uint8_t pin, bool val);

#endif //ARDUINO_AUTOPUMP_GPIO_H
