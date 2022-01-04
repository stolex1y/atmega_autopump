#ifndef ARDUINO_AUTOPUMP_LCD1602_H
#define ARDUINO_AUTOPUMP_LCD1602_H

#include <stdint.h>

#define E 11
#define RS 12

#define DATA_4 3
#define DATA_5 4
#define DATA_6 5
#define DATA_7 6

void lcd1602_init(void);
void lcd1602_set_cursor_pos(uint8_t x, uint8_t y);
void lcd1602_send_char(char ch);
void lcd1602_send_string(const char str[]);
void lcd1602_clear();

#endif //ARDUINO_AUTOPUMP_LCD1602_H
