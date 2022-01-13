#include "encoder.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>

#include "gpio.h"
#include "timer.h"

#define TIMER_PERIOD_US 50

static struct encoder* tim1_enc;
static void handle(struct encoder* encoder, enum encoder_state state);
static void timer1_vector_encoder();
static void pin_init(struct encoder_pins pins);

struct encoder encoder_create(struct encoder_pins pins) {
    struct encoder encoder = (struct encoder) { .pins = pins };
    return encoder;
}

void encoder_init(struct encoder* encoder) {
    pin_init(encoder->pins);
    tim1_enc = encoder;
    uint16_t hz = 1000000 / TIMER_PERIOD_US;
    timer1_init(hz, timer1_vector_encoder);
}

enum encoder_state encoder_get_curr_state(struct encoder* encoder) {
    if (encoder) return encoder->state;
    else return ENC_STATE_UNDEFINED;
}

//void encoder_add_handler(struct encoder* enc, enum encoder_state state, handler* adding_handler) {
//    if (!enc) return;
//    handler* curr_handler = enc->handlers[state];
//    if (!curr_handler) curr_handler = adding_handler;
//    else {
//
//    }
//}

static bool button_is_pressed(const struct encoder* encoder) {
    return !digital_read(encoder->pins.key);
}

static void update_button_press_debounce(struct encoder* e) {
    bool current_press = button_is_pressed(e);
    if (e->press_delay > 0)
        e->press_delay += TIMER_PERIOD_US;
    if (current_press != e->was_pressed) {
        if (e->press_delay >= PRESS_DELAY_US) {
            e->press_delay = 0;
            e->pressed = current_press;
        } else if (e->press_delay == 0)
            e->press_delay += TIMER_PERIOD_US;
    }
}

static bool button_is_released(const struct encoder* encoder) {
    return encoder->was_pressed && !encoder->pressed;
}

static bool button_is_hold(const struct encoder* encoder) {
    return encoder->was_pressed && encoder->pressed;
}

static uint8_t count_button_clicks(const struct encoder* encoder) {
    if (encoder->click_cnt && encoder->last_click_us >= CLICKS_DELAY_US)
        return encoder->click_cnt;
    return 0;
}

static void update_button_clicks(struct encoder* encoder) {
    if (button_is_released(encoder)) {
        if (encoder->last_click_us < CLICKS_DELAY_US) {
            if (!encoder->was_turned)
                encoder->click_cnt++;
            else {
                encoder->was_turned = false;
                encoder->click_cnt = 0;
            }
        } else {
            encoder->click_cnt = 1;
        }
        encoder->last_click_us = 0;
    } else if (encoder->last_click_us >= CLICKS_DELAY_US)
        encoder->click_cnt = 0;

    if (encoder->click_cnt == 0)
        encoder->last_click_us = 0;
    else
        encoder->last_click_us += TIMER_PERIOD_US;
}

static void timer1_vector_encoder() {
    tim1_enc->new_state =
            (digital_read(tim1_enc->pins.s1) << 1) | digital_read(tim1_enc->pins.s2);

    update_button_press_debounce(tim1_enc);
    update_button_clicks(tim1_enc);

    switch (tim1_enc->old_state | tim1_enc->new_state) {
        case 0x01:
//        case 0x0e:
            if (button_is_hold(tim1_enc)) {
                handle(tim1_enc, ENC_STATE_PRESS_N_TURN_RIGHT);
                tim1_enc->was_turned = true;
            } else
                handle(tim1_enc, ENC_STATE_TURN_RIGHT);
            break;
        case 0x04:
//        case 0x0b:
            if (button_is_hold(tim1_enc)) {
                handle(tim1_enc, ENC_STATE_PRESS_N_TURN_LEFT);
                tim1_enc->was_turned = true;
            } else
                handle(tim1_enc, ENC_STATE_TURN_LEFT);
            break;
        default:
            if (!tim1_enc->was_turned) {
                switch (count_button_clicks(tim1_enc)) {
                    case 1:
                        handle(tim1_enc, ENC_STATE_CLICK);
                        break;
                    case 2:
                        handle(tim1_enc, ENC_STATE_DOUBLE_CLICK);
                        break;
                }
            } else {
                tim1_enc->state = ENC_STATE_UNDEFINED;
            }
    }
    tim1_enc->old_state = tim1_enc->new_state << 2;
    tim1_enc->was_pressed = tim1_enc->pressed;
}

static void handle(struct encoder* encoder, enum encoder_state state) {
    encoder->state = state;
    if (encoder->handlers[state])
        encoder->handlers[state]();
}

static void pin_init(struct encoder_pins pins) {
    pin_mode(pins.s1, INPUT);
    pin_mode(pins.s2, INPUT);
    pin_mode(pins.key, INPUT_PULLUP);
}





