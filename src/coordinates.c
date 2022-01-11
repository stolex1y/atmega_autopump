#include "coordinates.h"

#include <stdint.h>

struct coordinates coordinates_create(coord_type x, coord_type y) {
    return (struct coordinates) { .x = x, .y = y };
}

struct coordinates coordinates_zeros() {
    return (struct coordinates) { .x = 0, .y = 0 };
}

/*struct coordinates {
    uint8_t x;
    uint8_t y;
    const uint8_t x_max;
    const uint8_t y_max;
};*/

/*
struct coordinates coordinates_create(uint8_t x_max, uint8_t y_max) {
    return (struct coordinates) {
        .x = 0,
        .y = 0,
        .x_max = x_max,
        .y_max = y_max
    };
}

void coordinates_set_x(struct coordinates* coordinates, uint8_t x) {
    if (x > coordinates->x_max)
        coordinates->x = coordinates->x_max;
    else if (x < 0)
        coordinates->x = 0;
    else
        coordinates->x = x;
}

void coordinates_set_y(struct coordinates* coordinates, uint8_t y) {
    if (y > coordinates->y_max)
        coordinates->y = coordinates->y_max;
    else if (y < 0)
        coordinates->y = 0;
    else
        coordinates->y = y;
}

void coordinates_set(struct coordinates* coordinates, uint8_t x, uint8_t y) {
    coordinates_set_x(coordinates, x);
    coordinates_set_y(coordinates, y);
}

uint8_t coordinates_get_x(struct coordinates* coordinates) {
    return coordinates->x;
}

uint8_t coordinates_get_y(struct coordinates* coordinates) {
    return coordinates->y;
}*/
