#ifndef ARDUINO_AUTOPUMP_TIME_H
#define ARDUINO_AUTOPUMP_TIME_H

#include <stdint.h>
#include <stdio.h>

enum time_part {
    TIME_PART_HR = 0,
    TIME_PART_MIN,
    TIME_PART_SEC,
    TIME_PART_LEN
};

struct time {
    uint8_t parts[3];
};

struct time time_create(uint8_t hr, uint8_t min, uint8_t sec);
struct time time_empty();
int time_to_string(struct time time, char* buf, size_t buf_len);
struct time time_from_string(const char* time_str);
uint64_t time_to_ms(struct time time);
void time_part_inc(struct time* time, enum time_part time_part);
void time_part_dec(struct time* time, enum time_part time_part);

#endif //ARDUINO_AUTOPUMP_TIME_H
