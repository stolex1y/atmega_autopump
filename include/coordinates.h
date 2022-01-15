#ifndef ARDUINO_AUTOPUMP_COORDINATES_H
#define ARDUINO_AUTOPUMP_COORDINATES_H

#include <stdint.h>

typedef uint8_t coordinates_type;

struct coordinates {
    coordinates_type x;
    coordinates_type y;
};

struct coordinates coordinates_create(coordinates_type x, coordinates_type y);
struct coordinates coordinates_zeros();

void coordinates_validate(
        struct coordinates* coordinates,
        struct coordinates min,
        struct coordinates max
);

#endif //ARDUINO_AUTOPUMP_COORDINATES_H
