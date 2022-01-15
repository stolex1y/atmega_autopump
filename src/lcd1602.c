#include "lcd1602.h"

#include <util/delay.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "gpio.h"
#include "coordinates.h"

struct lcd1602 {
    bool is_4bit;
    struct lcd1602_pins pins;
    struct coordinates curr_addr;
};

enum byte_mode {
    CMD = 0,
    DATA = 1
};

static void send_byte(const struct lcd1602* lcd, uint8_t byte, enum byte_mode mode);
static void send_cmd_byte(const struct lcd1602* lcd, uint8_t byte);
static void validate_coordinates(struct coordinates* coordinates);
static void lcd1602_init(struct lcd1602* lcd);

struct lcd1602* lcd1602_create(const bool is_4bit, const struct lcd1602_pins pins) {
    struct lcd1602* const lcd = malloc(sizeof(struct lcd1602));
    *lcd = (struct lcd1602) {
        .pins = pins,
        .is_4bit = is_4bit
    };
    lcd1602_init(lcd);
    return lcd;
}

void lcd1602_send_char(struct lcd1602* const lcd, const char ch) {
    send_byte(lcd, ch, DATA);
    if (lcd->curr_addr.x >= LCD_WIDTH - 1 && lcd->curr_addr.y < LCD_HEIGHT - 1) {
        lcd->curr_addr.y++;
        lcd->curr_addr.x = 0;
        lcd1602_set_cursor_pos(lcd, lcd->curr_addr);
    } else
        lcd->curr_addr.x++;
}

void lcd1602_set_cursor_pos(struct lcd1602* const lcd, struct coordinates coordinates) {
    validate_coordinates(&coordinates);
    send_cmd_byte(lcd, (coordinates.y * 0x40 + coordinates.x) | 0x80);
    lcd->curr_addr = coordinates;
}

void lcd1602_send_string(struct lcd1602* const lcd, const char str[]) {
    if (!str) return;
    char ch = str[0];
    for (size_t i = 0; ch; i++, ch = str[i]) {
        lcd1602_send_char(lcd, ch);
    }
}

void lcd1602_clear(struct lcd1602* const lcd) {
    send_cmd_byte(lcd, 0x01);
    lcd->curr_addr = coordinates_zeros();
}

void lcd1602_to_start_pos(struct lcd1602* const lcd) {
    lcd1602_set_cursor_pos(lcd, (struct coordinates) { .x = 0, .y = 0 });
    lcd->curr_addr = coordinates_zeros();
}

void lcd1602_clear_rect(
        struct lcd1602* const lcd,
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

static void validate_coordinates(struct coordinates* const coordinates) {
    coordinates_validate(
            coordinates,
            coordinates_zeros(),
            coordinates_create(LCD_WIDTH - 1, LCD_HEIGHT - 1)
    );
}

static void set_E(const struct lcd1602* const lcd) {
    digital_write(lcd->pins.E, 1);
}

static void set_RS(const struct lcd1602* const lcd) {
    digital_write(lcd->pins.RS, 1);
}

static void clear_E(const struct lcd1602* const lcd) {
    digital_write(lcd->pins.E, 0);
}

static void clear_RS(const struct lcd1602* const lcd) {
    digital_write(lcd->pins.RS, 0);
}

static void set_data(const struct lcd1602* const lcd, const uint8_t half_byte) {
    digital_write(lcd->pins.DATA_4, half_byte & 1);
    digital_write(lcd->pins.DATA_5, (half_byte >> 1) & 1);
    digital_write(lcd->pins.DATA_6, (half_byte >> 2) & 1);
    digital_write(lcd->pins.DATA_7, (half_byte >> 3) & 1);
}

static void send_half_byte(const struct lcd1602* const lcd, const uint8_t half_byte) {
    set_E(lcd);
    _delay_ms(1);
    set_data(lcd, half_byte);
    clear_E(lcd);
    _delay_ms(1);
}

static void send_byte(const struct lcd1602* const lcd, const uint8_t byte, const enum byte_mode mode) {
    (mode == CMD) ? clear_RS(lcd) : set_RS(lcd);
    send_half_byte(lcd, byte >> 4);
    send_half_byte(lcd, byte);
}

static void send_cmd_byte(const struct lcd1602* const lcd, uint8_t byte) {
    send_byte(lcd, byte, CMD);
}

static void pin_init(const struct lcd1602* const lcd) {
    //data pins
    pin_mode(lcd->pins.DATA_4, OUTPUT);
    pin_mode(lcd->pins.DATA_5, OUTPUT);
    pin_mode(lcd->pins.DATA_6, OUTPUT);
    pin_mode(lcd->pins.DATA_7, OUTPUT);

    //E
    pin_mode(lcd->pins.E, OUTPUT);
    //RS
    pin_mode(lcd->pins.RS, OUTPUT);
}

static void lcd1602_init(struct lcd1602* const lcd) {
    // lcd init
    pin_init(lcd);
    _delay_ms(15);
    send_half_byte(lcd, 0x3);
    _delay_ms(5);
    send_half_byte(lcd, 0x3);
    _delay_ms(1);
    send_half_byte(lcd, 0x3);

    // lcd settings
    // DB4 = 0 - 4-bit data length and DB3 = 1 - 2 lines
    send_half_byte(lcd, 0x2);
    send_cmd_byte(lcd, 0x28);
    // set display (DB2 = 1), cursor and blinking cursor (DB1 = 0, DB0 = 0)
    send_cmd_byte(lcd, 0x0C);
    // DB1 = 1 - cursor moves to right, DB0 = 0 - shifting of entire isn't performed
    send_cmd_byte(lcd, 0x06);

    lcd1602_clear(lcd);
    lcd->curr_addr = coordinates_zeros();
}

