#include "coordinates.h"

struct coordinates coordinates_create(const coordinates_type x, const coordinates_type y) {
    return (struct coordinates) { .x = x, .y = y };
}

struct coordinates coordinates_zeros() {
    return (struct coordinates) { .x = 0, .y = 0 };
}

void coordinates_validate(
        struct coordinates* const coordinates,
        const struct coordinates min,
        const struct coordinates max
) {
    if (coordinates->x > max.x) coordinates->x = max.x;
    else if (coordinates->x < min.x) coordinates->x = min.x;

    if (coordinates->y >= max.y) coordinates->y = max.y;
    else if (coordinates->y < min.y) coordinates->y = min.y;
}
