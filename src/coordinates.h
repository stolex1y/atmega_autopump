#ifndef ARDUINO_AUTOPUMP_COORDINATES_H
#define ARDUINO_AUTOPUMP_COORDINATES_H

#include <stdint.h>

typedef uint8_t coord_type;

struct coordinates {
    coord_type x;
    coord_type y;
};

struct coordinates coordinates_create(coord_type x, coord_type y);

struct coordinates coordinates_zeros();

#endif //ARDUINO_AUTOPUMP_COORDINATES_H
