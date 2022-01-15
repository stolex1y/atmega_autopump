#include "../include/uptime.h"

#include <avr/interrupt.h>

#include <stdint.h>

#include "../include/timer.h"

static volatile uint64_t millis_value = 0;

static void timer0_comp_isr() {
    millis_value++;
}

void uptime_init() {
    timer0_init(timer0_comp_isr);
}

uint64_t millis() {
    uint64_t m;
    cli();
    m = millis_value;
    sei();

    return m;
};

