#include "lcd1602.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "gpio.h"
#include "coordinates.h"

enum byte_mode {
    CMD = 0,
    DATA = 1
};

static void send_half_byte(struct lcd1602* lcd, uint8_t half_byte);
static void send_byte(struct lcd1602* lcd, uint8_t byte, enum byte_mode mode);
static void send_cmd_byte(struct lcd1602* lcd, uint8_t byte);
static void pin_init(struct lcd1602* lcd);

static void set_E(struct lcd1602* lcd) {
    digital_write(lcd->E, 1);
}

static void set_RS(struct lcd1602* lcd) {
    digital_write(lcd->RS, 1);
}

static void clear_E(struct lcd1602* lcd) {
    digital_write(lcd->E, 0);
}

static void clear_RS(struct lcd1602* lcd) {
    digital_write(lcd->RS, 0);
}

static void validate_coordinates(struct coordinates* coordinates) {
    if (coordinates->x >= LCD_WIDTH) coordinates->x = LCD_WIDTH - 1;
    else if (coordinates->x < 0) coordinates->x = 0;

    if (coordinates->y >= LCD_HEIGHT) coordinates->y = LCD_HEIGHT - 1;
    else if (coordinates->y < 0) coordinates->y = 0;
}

void lcd1602_init(struct lcd1602* lcd) {
    // lcd init
    pin_init(lcd);
    _delay_ms(15);
    send_half_byte(lcd, 0b11);
    _delay_ms(5);
    send_half_byte(lcd, 0b11);
    _delay_ms(1);
    send_half_byte(lcd, 0b11);

    // lcd settings
    // DB4 = 0 - 4-bit data length and DB3 = 1 - 2 lines
    send_half_byte(lcd, 0b10);
    send_cmd_byte(lcd, 0b00101000);
    // set display (DB2 = 1), cursor and blinking cursor (DB1 = 0, DB0 = 0)
    send_cmd_byte(lcd, 0b00001100);
    // DB1 = 1 - cursor moves to right, DB0 = 0 - shifting of entire isn't performed
    send_cmd_byte(lcd, 0b00000110);

    lcd1602_clear(lcd);
    lcd->curr_addr = coordinates_zeros();
}

void lcd1602_send_char(struct lcd1602* lcd, char ch) {
    send_byte(lcd, ch, DATA);
    if (lcd->curr_addr.x >= LCD_WIDTH - 1 && lcd->curr_addr.y < LCD_HEIGHT - 1) {
        lcd->curr_addr.y++;
        lcd->curr_addr.x = 0;
        lcd1602_set_cursor_pos(lcd, lcd->curr_addr);
    } else
        lcd->curr_addr.x++;
}

void lcd1602_set_cursor_pos(struct lcd1602* lcd, struct coordinates coordinates) {
    validate_coordinates(&coordinates);
    send_cmd_byte(lcd, (coordinates.y * 0x40 + coordinates.x) | 0x80);
    lcd->curr_addr = coordinates;
}

void lcd1602_send_string(struct lcd1602* lcd, const char str[]) {
    if (!str) return;
    char ch = str[0];
    for (size_t i = 0; ch; i++, ch = str[i]) {
        lcd1602_send_char(lcd, ch);
    }
}

void lcd1602_clear(struct lcd1602* lcd) {
    send_cmd_byte(lcd, 0x01);
}

void lcd1602_to_start_pos(struct lcd1602* lcd) {
    lcd1602_set_cursor_pos(lcd, (struct coordinates) { .x = 0, .y = 0 });
    lcd->curr_addr = coordinates_zeros();
}

void lcd1602_clear_rect(
        struct lcd1602* lcd,
        struct coordinates coord0,
        struct coordinates coord1
) {
    if (coord1.y < coord0.y || (coord1.y == coord0.y && coord1.x < coord0.x)) return;

    validate_coordinates(&coord0);
    validate_coordinates(&coord1);

    struct coordinates current_coord;
    for (current_coord.y = coord0.y; current_coord.y <= coord1.y; current_coord.y++) {
        current_coord.x = coord0.x;
        lcd1602_set_cursor_pos(lcd, current_coord);
        while (current_coord.x <= coord1.x) {
            lcd1602_send_char(lcd, ' ');
            current_coord.x++;
        }
    }
}

static void set_data(struct lcd1602* lcd, uint8_t half_byte) {
    digital_write(lcd->DATA_4, half_byte & 1);
    digital_write(lcd->DATA_5, (half_byte >> 1) & 1);
    digital_write(lcd->DATA_6, (half_byte >> 2) & 1);
    digital_write(lcd->DATA_7, (half_byte >> 3) & 1);
}

static void send_half_byte(struct lcd1602* lcd, uint8_t half_byte) {
    set_E(lcd);
    _delay_ms(1);
    set_data(lcd, half_byte);
    clear_E(lcd);
    _delay_ms(1);
}

static void send_byte(struct lcd1602* lcd, uint8_t byte, enum byte_mode mode) {
    (mode == CMD) ? clear_RS(lcd) : set_RS(lcd);
    send_half_byte(lcd, byte >> 4);
    send_half_byte(lcd, byte);
}

static void send_cmd_byte(struct lcd1602* lcd, uint8_t byte) {
    send_byte(lcd, byte, CMD);
}

static void pin_init(struct lcd1602* lcd) {
    //data pins
    pin_mode(lcd->DATA_4, OUTPUT);
    pin_mode(lcd->DATA_5, OUTPUT);
    pin_mode(lcd->DATA_6, OUTPUT);
    pin_mode(lcd->DATA_7, OUTPUT);

    //E
    pin_mode(lcd->E, OUTPUT);
    //RS
    pin_mode(lcd->RS, OUTPUT);
}

