#include "../include/timer.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdint.h>

#include "../include/gpio.h"

struct timer {
    bool is_busy;
    timer_vector* vector;
};

static void stub_vector() {}

static struct timer timers[] = {
    [0] = (struct timer) { false, stub_vector },
    [1] = (struct timer) { false, stub_vector }
};

ISR(TIMER1_COMPA_vect) {
    timers[1].vector();
}

ISR(TIMER0_COMPA_vect) {
    timers[0].vector();
}

static void timer0_clean() {
    reg_clear(TCCR0A);
    reg_clear(TCCR0B);
    reg_clear(TIMSK0);
}

void timer0_init(timer_vector* timer0_vector) {
    timers[0].is_busy = true;
    timer0_clean();

    bit_set(TCCR0A, WGM01); // set mode CTC;

    bit_set(TCCR0B, CS01); // clk / 64
    bit_set(TCCR0B, CS00);
    TCNT0 = 0x00;
    OCR0A = (F_CPU >> 6) / 1000 - 1; // every 1000 us

    bit_set(TIMSK0, OCIE0A); // enable timer interrupt

    timers[0].vector = timer0_vector;
}

static void timer1_clean() {
    reg_clear(TIMSK1);
    reg16_write(OCR1AH, OCR1AL, 0x0);
    reg_clear(TCCR1B);
}

void timer1_init(uint16_t hz, timer_vector* timer1_vector) {
    if (timers[1].is_busy) return;
    timers[1].is_busy = true;

    timer1_clean();

    bit_set(TCCR1B, WGM12); // set mode CTC
    bit_set(TIMSK1, OCIE1A); // enable timer interrupt

    uint16_t compare = (uint16_t) (F_CPU >> 8) / hz;
    reg16_write(OCR1AH, OCR1AL, compare);

    bit_set(TCCR1B, CS12); // clk / 256

    if (timer1_vector)
        timers[1].vector = timer1_vector;
}

void timer1_free() {
    timer1_clean();
    timers[1].vector = stub_vector;
    timers[1].is_busy = false;
}


