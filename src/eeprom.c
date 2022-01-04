#include "eeprom.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <util/delay.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "gpio.h"

void eeprom_write_byte(uint16_t addr, uint8_t data) {
    // Wait for completion of previous write
    loop_until_bit_is_clear(EECR, EEPE);

    EEAR = addr;
    EEDR = data;
    bit_set(EECR, EEMPE);
    // Start eeprom write by setting EEPE
    bit_set(EECR, EEPE);
}

uint8_t eeprom_read_byte(uint16_t addr) {
    // Wait for completion of previous write
    loop_until_bit_is_clear(EECR, EEPE);

    EEAR = addr;
    // Start eeprom read by writing EERE
    bit_set(EECR, EERE);
    return EEDR;
}

uint16_t eeprom_read_string(uint16_t addr, char str[], const uint16_t size) {
    uint16_t curr_size = 0;
    if (!str || !size || addr >= EEPROM_SIZE) return 0;
    char curr_ch = eeprom_read_byte(addr++);
    while (curr_ch != 0 && curr_size < size && addr < EEPROM_SIZE) {
        str[curr_size++] = curr_ch;
        curr_ch = eeprom_read_byte(addr++);
    }
    if (curr_size < size && addr < EEPROM_SIZE)
        str[curr_size] = 0;
    return curr_size;

}

uint16_t eeprom_write_string(uint16_t addr, const char str[]) {
    if (!str || addr >= EEPROM_SIZE) return 0;
    uint16_t ch_cnt = 0;
    char ch = str[ch_cnt];
    while (ch != 0 && addr < EEPROM_SIZE) {
        ch_cnt++;
        eeprom_write_byte(addr++, ch);
        ch = str[ch_cnt];
    }
    if (addr < EEPROM_SIZE) eeprom_write_byte(addr, 0);
    return ch_cnt;
}

void eeprom_clean(const uint16_t addr_start, const uint16_t addr_end) {
    for (uint16_t i = addr_start; i < addr_end && i < EEPROM_SIZE; i++)
        eeprom_write_byte(i, 0xFF);
}


