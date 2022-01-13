#include "date.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>

int date_to_string(struct date date, char* buf, size_t buf_len) {
    return snprintf(buf, buf_len, "%02" PRIu8 ":%02" PRIu8 ":%02" PRIu8,
            date.parts[DATE_PART_HR],
            date.parts[DATE_PART_MIN],
            date.parts[DATE_PART_SEC]
    );
}

struct date date_from_string(const char* date_str) {
    struct date date = { 0 };
    char part[2];
    for (size_t part_i = 0; part_i < 3; part_i++) {
        for (size_t i = 0; i < 2; i++) {
            part[i] = date_str[part_i * 3 + i];
        }
        date.parts[part_i] = strtol(part, NULL, 10);
    }
    return date;
}

struct date date_create(uint8_t hr, uint8_t min, uint8_t sec) {
    return (struct date) {
            .parts = {
                    [DATE_PART_HR] = hr,
                    [DATE_PART_MIN] = min,
                    [DATE_PART_SEC] = sec
            }
    };
}

struct date date_empty() {
    return date_create(0, 0, 0);
}

void date_part_inc(struct date* date, enum date_part date_part) {
    if (!date) return;
    uint8_t date_part_max = 59;
    if (date_part == DATE_PART_HR)
        date_part_max = UINT8_MAX;
    if (date->parts[date_part] + 1 <= date_part_max)
        date->parts[date_part]++;
}

void date_part_dec(struct date* date, enum date_part date_part) {
    if (!date) return;
    uint8_t date_part_min = 0;
    if (date->parts[date_part] - 1 >= date_part_min)
        date->parts[date_part]--;
}