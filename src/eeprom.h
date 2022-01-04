#ifndef ARDUINO_AUTOPUMP_EEPROM_H
#define ARDUINO_AUTOPUMP_EEPROM_H

#include <stdint.h>

#ifndef EEPROM_SIZE
#define EEPROM_SIZE 512
#endif //EEPROM_SIZE

void eeprom_write_byte(uint16_t addr, uint8_t data);
uint8_t  eeprom_read_byte(uint16_t addr);
uint16_t eeprom_write_string(uint16_t addr, const char str[]);
uint16_t eeprom_read_string(uint16_t addr, char str[], const uint16_t size);
void eeprom_clean();

#endif //ARDUINO_AUTOPUMP_EEPROM_H
