#ifndef ARDUINO_AUTOPUMP_PLANT_H
#define ARDUINO_AUTOPUMP_PLANT_H

#include <stdio.h>
#include <stdbool.h>

#include "date.h"

enum plant_setting {
    NAME = 0,
    DELAY,
    PUMP,
    PLANT_STTNG_LEN
};

static const char* plant_setting_str[] = {
    [NAME] = "Name",
    [DELAY] = "Delay",
    [PUMP] = "Pump"
};

struct plant {
    const char* name;
    struct date delay;
    struct date pump;
    uint8_t pump_pin;
};

struct plant plant_create(const char* name, uint8_t pump_pin);
bool plant_get_date_setting(struct plant plant, enum plant_setting setting, struct date* date);
void plant_set_date_setting(struct plant* plant, enum plant_setting setting, struct date new_date_setting);

#endif //ARDUINO_AUTOPUMP_PLANT_H
