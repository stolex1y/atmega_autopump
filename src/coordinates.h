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

//void coordinates_set_x(struct coordinates* coordinates, uint8_t x);
//void coordinates_set_y(struct coordinates* coordinates, uint8_t y);
//void coordinates_set(struct coordinates* coordinates, uint8_t x, uint8_t y);
//void coordinates_get_x(struct coordinates* coordinates);
//void coordinates_get_y(struct coordinates* coordinates);

#endif //ARDUINO_AUTOPUMP_COORDINATES_H
