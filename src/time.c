#include "time.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>

#define HOURS_TO_MS (uint64_t) 3600000
#define MINUTES_TO_MS (uint64_t) 60000
#define SECONDS_TO_MS (uint64_t) 1000
#define SECONDS_MAX 59

struct time time_create(const uint8_t hr, const uint8_t min, const uint8_t sec) {
    return (struct time) {
            .parts = {
                    [TIME_PART_HR] = hr,
                    [TIME_PART_MIN] = min,
                    [TIME_PART_SEC] = sec
            }
    };
}

struct time time_empty() {
    return time_create(0, 0, 0);
}

int time_to_string(const struct time time, char* const buf, const size_t buf_len) {
    return snprintf(buf, buf_len, "%02" PRIu8 ":%02" PRIu8 ":%02" PRIu8,
                    time.parts[TIME_PART_HR],
                    time.parts[TIME_PART_MIN],
                    time.parts[TIME_PART_SEC]
    );
}

struct time time_from_string(const char* const time_str) {
    struct time time = time_empty();
    char part[2];
    for (size_t part_i = 0; part_i < 3; part_i++) {
        for (size_t i = 0; i < 2; i++) {
            part[i] = time_str[part_i * 3 + i];
        }
        time.parts[part_i] = strtol(part, NULL, 10);
    }
    return time;
}

uint64_t time_to_ms(const struct time time) {
    uint64_t millis = 0;
    millis += (uint64_t) time.parts[TIME_PART_HR] * HOURS_TO_MS;
    millis += (uint64_t) time.parts[TIME_PART_MIN] * MINUTES_TO_MS;
    millis += (uint64_t) time.parts[TIME_PART_SEC] * SECONDS_TO_MS;
    return millis;
}

void time_part_inc(struct time* const time, const enum time_part time_part) {
    if (!time) return;
    const uint8_t time_part_max = time_part == TIME_PART_HR ? UINT8_MAX : SECONDS_MAX;
    if (time->parts[time_part] + 1 <= time_part_max)
        time->parts[time_part]++;
}

void time_part_dec(struct time* const time, const enum time_part time_part) {
    if (!time) return;
    const uint8_t time_part_min = 0;
    if (time->parts[time_part] - 1 >= time_part_min)
        time->parts[time_part]--;
}
