#ifndef ARDUINO_AUTOPUMP_LCD1602_H
#define ARDUINO_AUTOPUMP_LCD1602_H

#include <stdint.h>
#include <stdbool.h>

#include "coordinates.h"

#define LCD_WIDTH 16
#define LCD_HEIGHT 2

struct lcd1602_pins {
    uint8_t E;
    uint8_t RS;
    uint8_t DATA_0;
    uint8_t DATA_1;
    uint8_t DATA_2;
    uint8_t DATA_3;
    uint8_t DATA_4;
    uint8_t DATA_5;
    uint8_t DATA_6;
    uint8_t DATA_7;
};

struct lcd1602;

struct lcd1602* lcd1602_create(bool is_4bit, struct lcd1602_pins pins);
void lcd1602_set_cursor_pos(struct lcd1602* lcd, struct coordinates coordinates);
void lcd1602_send_char(struct lcd1602* lcd, char ch);
void lcd1602_send_string(struct lcd1602* lcd, const char str[]);
void lcd1602_clear(struct lcd1602* lcd);
void lcd1602_to_start_pos(struct lcd1602* lcd);
void lcd1602_clear_rect(
        struct lcd1602* lcd,
        struct coordinates coord0,
        struct coordinates coord1
);

#endif //ARDUINO_AUTOPUMP_LCD1602_H
