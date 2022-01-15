#include "plant.h"

#include <stdbool.h>

#include "time.h"

static const char* const plant_setting_str[] = {
        [NAME] = "Name",
        [DELAY] = "Delay",
        [PUMP] = "Pump"
};

struct plant plant_create(const char* const name, const uint8_t pump_pin) {
    return (struct plant) {
        .name = name,
        .delay = time_empty(),
        .pump = time_empty(),
        .pump_pin = pump_pin
    };
}

bool plant_get_time_setting(const struct plant plant, const enum plant_setting setting, struct time* const time) {
    switch (setting) {
        case DELAY:
            *time = plant.delay;
            return true;
        case PUMP:
            *time = plant.pump;
            return true;
        default:
            return false;
    }
}

void plant_set_time_setting(struct plant* const plant, const enum plant_setting setting, const struct time new_time_setting) {
    switch (setting) {
        case DELAY:
            plant->delay = new_time_setting;
            break;
        case PUMP:
            plant->pump = new_time_setting;
            break;
        default:
            break;
    }
}

const char* plant_setting_to_str(const enum plant_setting setting) {
    return plant_setting_str[setting];
}

