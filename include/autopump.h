#ifndef ARDUINO_AUTOPUMP_AUTOPUMP_H
#define ARDUINO_AUTOPUMP_AUTOPUMP_H

#include <inttypes.h>
#include <stdio.h>

#include "lcd1602.h"
#include "encoder.h"
#include "plant.h"

void autopump_init(struct lcd1602* lcd, struct encoder* enc, const struct plant plants[], size_t plants_len);
void autopump_timers_update();

#endif //ARDUINO_AUTOPUMP_AUTOPUMP_H
