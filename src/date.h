#ifndef ARDUINO_AUTOPUMP_DATE_H
#define ARDUINO_AUTOPUMP_DATE_H

#include <stdint.h>
#include <stdio.h>

struct date {
    uint8_t parts[3];
};

enum date_part {
    DATE_PART_HR = 0,
    DATE_PART_MIN,
    DATE_PART_SEC,
    DATE_PART_LEN
};

int date_to_string(struct date date, char* buf, size_t buf_len);
struct date date_from_string(const char* date_str);
struct date date_empty();
void date_part_inc(struct date* date, enum date_part date_part);
void date_part_dec(struct date* date, enum date_part date_part);

#endif //ARDUINO_AUTOPUMP_DATE_H
