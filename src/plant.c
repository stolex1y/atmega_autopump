#include "../include/plant.h"

#include <stdbool.h>

#include "../include/date.h"

struct plant plant_create(const char* name, uint8_t pump_pin) {
    return (struct plant) {
        .name = name,
        .delay = date_empty(),
        .pump = date_empty(),
        .pump_pin = pump_pin
    };
}

bool plant_get_date_setting(struct plant plant, enum plant_setting setting, struct date* date) {
    switch (setting) {
        case DELAY:
            *date = plant.delay;
            return true;
        case PUMP:
            *date = plant.pump;
            return true;
        default:
            return false;
    }
}

void plant_set_date_setting(struct plant* plant, enum plant_setting setting, struct date new_date_setting) {
    switch (setting) {
        case DELAY:
            plant->delay = new_date_setting;
            break;
        case PUMP:
            plant->pump = new_date_setting;
            break;
        default:
            break;
    }
}

