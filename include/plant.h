#ifndef ARDUINO_AUTOPUMP_PLANT_H
#define ARDUINO_AUTOPUMP_PLANT_H

#include <stdio.h>
#include <stdbool.h>

#include "time.h"

enum plant_setting {
    NAME = 0,
    DELAY,
    PUMP,
    PLANT_STTNG_LEN
};

struct plant {
    const char* name;
    struct time delay;
    struct time pump;
    uint8_t pump_pin;
};

struct plant plant_create(const char* name, uint8_t pump_pin);
bool plant_get_time_setting(struct plant plant, enum plant_setting setting, struct time* time);
void plant_set_time_setting(struct plant* plant, enum plant_setting setting, struct time new_time_setting);
const char* plant_setting_to_str(enum plant_setting setting);

#endif //ARDUINO_AUTOPUMP_PLANT_H
