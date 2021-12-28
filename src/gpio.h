#ifndef ARDUINO_AUTOPUMP_GPIO_H
#define ARDUINO_AUTOPUMP_GPIO_H

#include <stdbool.h>
#include <stdint.h>

#define HIGH 0x1
#define LOW 0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

void pin_mode(uint8_t pin, uint8_t mode);
void digital_write(uint8_t pin, bool val);
bool digital_read(uint8_t pin);
void digital_toggle(uint8_t pin);
bool analog_read(uint8_t pin);
void analog_write(uint8_t pin, bool val);

#endif //ARDUINO_AUTOPUMP_GPIO_H
