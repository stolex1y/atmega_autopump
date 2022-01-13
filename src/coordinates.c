#include "coordinates.h"

struct coordinates coordinates_create(coord_type x, coord_type y) {
    return (struct coordinates) { .x = x, .y = y };
}

struct coordinates coordinates_zeros() {
    return (struct coordinates) { .x = 0, .y = 0 };
}