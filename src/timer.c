#include "timer.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdint.h>

#include "gpio.h"

static void stub_vector() {
    //stub
}

timer_vector* timer1_compa = stub_vector;

ISR(TIMER1_COMPA_vect) {
    timer1_compa();
}

//max 65535
// 1/256 1..62500
// 1/64  62500..250000
void timer1_init(uint8_t hz, timer_vector* timer1_vector) {
    bit_set(TCCR1B, WGM12); // set mode CTC
    bit_set(TIMSK1, OCIE1A); // enable timer interrupt

    uint16_t compare = (F_CPU >> 8) / hz;
    reg16_write(OCR1AH, OCR1AL, compare);

    bit_set(TCCR1B, CS12); // clk / 256

    if (timer1_vector)
        timer1_compa = *timer1_vector;
}
