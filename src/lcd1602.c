#include "lcd1602.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "gpio.h"

/**
* 4-битный формат: D3..D6
 * E - D11
 * RS - D12
*/
#define set_E digital_write(E, 1)
#define clear_E digital_write(E, 0)
#define set_RS digital_write(RS, 1)
#define clear_RS digital_write(RS, 0)

enum byte_mode {
    CMD = 0,
    DATA = 1
};

static void send_half_byte(uint8_t half_byte);
static void send_byte(uint8_t byte, enum byte_mode mode);
static void send_cmd_byte(uint8_t byte);
static void pin_init(void);

void lcd1602_init(void) {
    pin_init();
    _delay_ms(15);
    send_half_byte(0x3);
    _delay_ms(100);
    send_half_byte(0x3);
    _delay_ms(1);
    send_half_byte(0x2);
    _delay_ms(1);

    //DB4=0 - 4-bit data length and DB3=1 - 2 lines
    send_cmd_byte(0b00101000);
    _delay_ms(1);

    //set display (DB2=1), cursor and blinking cursor (DB1=0, DB0=0)
    send_cmd_byte(0b00001100);
    _delay_ms(1);

    //DB1=1 - cursor moves to right, DB0=0 - shifting of entire isn't performed
    send_cmd_byte(0b00000110);
    _delay_ms(1);
}

void lcd1602_send_char(char ch) {
    send_byte(ch, DATA);
}

void lcd1602_set_cursor_pos(uint8_t x, uint8_t y) {
    send_cmd_byte((y * 0x40 + x) | 0x80);
}

void lcd1602_send_string(const char str[]) {
    if (!str) return;
    char ch = str[0];
    for (size_t i = 0; ch; i++, ch = str[i])
        lcd1602_send_char(ch);
}

static void set_data(uint8_t half_byte) {
    digital_write(D4, half_byte & 1);
    digital_write(D5, (half_byte >> 1) & 1);
    digital_write(D6, (half_byte >> 2) & 1);
    digital_write(D7, (half_byte >> 3) & 1);
}

static void send_half_byte(uint8_t half_byte) {
    set_E;
    _delay_ms(15);
    set_data(half_byte);
    clear_E;
    _delay_ms(50);
}

static void send_byte(uint8_t byte, enum byte_mode mode) {
    (mode == CMD) ? clear_RS : set_RS;
    send_half_byte(byte >> 4);
    send_half_byte(byte);
}

static void send_cmd_byte(uint8_t byte) {
    send_byte(byte, CMD);
}

static void pin_init(void) {
    //data pins
    pin_mode(3, OUTPUT);
    pin_mode(4, OUTPUT);
    pin_mode(5, OUTPUT);
    pin_mode(6, OUTPUT);

    //E
    pin_mode(11, OUTPUT);
    //RS
    pin_mode(12, OUTPUT);
}

