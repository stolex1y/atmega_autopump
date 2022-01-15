#include "encoder.h"

#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#include "gpio.h"
#include "timer.h"
#include "uptime.h"

#define SEC_TO_US 1000000
#define MS_TO_US 1000
#define TIMER_PERIOD_US 50

struct encoder {
    bool pressed;
    bool was_pressed;
    bool was_turned;
    uint8_t click_cnt;
    uint32_t last_click_us;
    uint32_t press_delay;
    uint32_t hold_millis_start;

    uint8_t new_state;
    uint8_t old_state;

    encoder_handler* handlers[ENC_STATE_LEN];

    struct encoder_pins pins;
    enum encoder_state state;
};

static struct encoder* tim1_enc;

static void timer1_vector_encoder();
static void pin_init(struct encoder_pins pins);
static void encoder_init(struct encoder* encoder);

struct encoder* encoder_create(const struct encoder_pins pins) {
    struct encoder* const encoder = malloc(sizeof(struct encoder));
    *encoder = (struct encoder) { .pins = pins };
    encoder_init(encoder);
    return encoder;
}

void encoder_set_handler(struct encoder* const encoder, const enum encoder_state state, encoder_handler handler) {
    encoder->handlers[state] = handler;
}

void encoder_init(struct encoder* const encoder) {
    const uint32_t hz32 = (uint32_t) SEC_TO_US / (uint32_t) TIMER_PERIOD_US;
    const uint16_t hz = hz32 > UINT16_MAX ? UINT16_MAX : (uint16_t) hz32;
    tim1_enc = encoder;
    pin_init(encoder->pins);
    timer1_init(hz, timer1_vector_encoder);
}

enum encoder_state encoder_get_curr_state(const struct encoder* const encoder) {
    if (encoder) return encoder->state;
    else return ENC_STATE_UNDEFINED;
}

static void pin_init(struct encoder_pins pins) {
    pin_mode(pins.s1, INPUT);
    pin_mode(pins.s2, INPUT);
    pin_mode(pins.key, INPUT_PULLUP);
}

static bool button_is_pressed(const struct encoder* const encoder) {
    return !digital_read(encoder->pins.key);
}

static void button_update_press_debounce(struct encoder* const e) {
    const bool current_press = button_is_pressed(e);
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

static bool button_is_released(const struct encoder* const encoder) {
    return encoder->was_pressed && !encoder->pressed;
}

static bool button_is_hold(const struct encoder* const encoder) {
    return encoder->was_pressed && encoder->pressed;
}

static uint32_t button_hold_millis(const struct encoder* const encoder) {
    if (encoder->hold_millis_start == 0) return 0;
    else return millis() - encoder->hold_millis_start;
}

static uint8_t button_count_clicks(const struct encoder* const encoder) {
    if (encoder->click_cnt && encoder->last_click_us >= CLICKS_DELAY_MS * (uint32_t) MS_TO_US)
        return encoder->click_cnt;
    return 0;
}

static void button_update_clicks(struct encoder* const encoder) {
    if (button_is_released(encoder)) {
        if (encoder->last_click_us < CLICKS_DELAY_MS * (uint32_t) MS_TO_US) {
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
    } else if (encoder->last_click_us >= CLICKS_DELAY_MS * (uint32_t) MS_TO_US)
        encoder->click_cnt = 0;

    if (encoder->click_cnt == 0)
        encoder->last_click_us = 0;
    else
        encoder->last_click_us += (uint32_t) TIMER_PERIOD_US;
}

static void handle(struct encoder* const encoder, const enum encoder_state state) {
    encoder->state = state;
    if (encoder->handlers[state])
        encoder->handlers[state]();
}

static bool encoder_turned_right(const struct encoder* const encoder) {
    const uint8_t old_new_state = encoder->old_state | encoder->new_state;
    if (old_new_state == 0x01)
        return true;
    else
        return false;
}

static bool encoder_turned_left(const struct encoder* const encoder) {
    const uint8_t old_new_state = encoder->old_state | encoder->new_state;
    if (old_new_state == 0x04)
        return true;
    else
        return false;
}

static void timer1_vector_encoder() {
    tim1_enc->new_state =
            (digital_read(tim1_enc->pins.s1) << 1) | digital_read(tim1_enc->pins.s2);

    button_update_press_debounce(tim1_enc);
    button_update_clicks(tim1_enc);

    if (button_is_hold(tim1_enc) && tim1_enc->hold_millis_start == 0) {
        tim1_enc->hold_millis_start = millis();
    } else if (!button_is_hold(tim1_enc))
        tim1_enc->hold_millis_start = 0;

    if (encoder_turned_right(tim1_enc)) {
        if (button_is_hold(tim1_enc)) {
            handle(tim1_enc, ENC_STATE_PRESS_N_TURN_RIGHT);
            tim1_enc->was_turned = true;
        } else
            handle(tim1_enc, ENC_STATE_TURN_RIGHT);
    } else if (encoder_turned_left(tim1_enc)) {
        if (button_is_hold(tim1_enc)) {
            handle(tim1_enc, ENC_STATE_PRESS_N_TURN_LEFT);
            tim1_enc->was_turned = true;
        } else
            handle(tim1_enc, ENC_STATE_TURN_LEFT);
    } else if (tim1_enc->was_turned) {
        tim1_enc->state = ENC_STATE_UNDEFINED;
    } else {
        switch (button_count_clicks(tim1_enc)) {
            case 0:
                if (button_hold_millis(tim1_enc) >= HOLD_MS)
                    handle(tim1_enc, ENC_STATE_HOLD);
                break;
            case 1:
                handle(tim1_enc, ENC_STATE_CLICK);
                break;
            case 2:
                handle(tim1_enc, ENC_STATE_DOUBLE_CLICK);
                break;
            default:
                tim1_enc->state = ENC_STATE_UNDEFINED;
        }
    }
    tim1_enc->old_state = tim1_enc->new_state << 2;
    tim1_enc->was_pressed = tim1_enc->pressed;
}





