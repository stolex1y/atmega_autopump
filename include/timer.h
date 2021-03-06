#ifndef ARDUINO_AUTOPUMP_TIMER_H
#define ARDUINO_AUTOPUMP_TIMER_H

#include <stdint.h>

typedef void (timer_vector)(void);

void timer1_init(uint16_t hz, timer_vector* timer1_vector);
void timer1_free();
void timer0_init(timer_vector* timer0_vector);

#endif //ARDUINO_AUTOPUMP_TIMER_H
