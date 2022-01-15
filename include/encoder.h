#ifndef ARDUINO_AUTOPUMP_ENCODER_H
#define ARDUINO_AUTOPUMP_ENCODER_H

#include <stdint.h>
#include <stdbool.h>

#include "pins.h"

#define CLICKS_DELAY_MS (uint32_t) 200
#define PRESS_DELAY_US 500
#define HOLD_MS 5000

typedef void encoder_handler(void);

enum encoder_state {
    ENC_STATE_TURN_RIGHT,
    ENC_STATE_TURN_LEFT,
    ENC_STATE_PRESS_N_TURN_LEFT,
    ENC_STATE_PRESS_N_TURN_RIGHT,
    ENC_STATE_CLICK,
    ENC_STATE_DOUBLE_CLICK,
    ENC_STATE_HOLD,
    ENC_STATE_UNDEFINED,
    ENC_STATE_LEN
};

struct encoder_pins {
    uint8_t s1;
    uint8_t s2;
    uint8_t key;
};

struct encoder;

struct encoder* encoder_create(struct encoder_pins pins);
enum encoder_state encoder_get_curr_state(const struct encoder* encoder);
void encoder_set_handler(struct encoder* encoder, enum encoder_state state, encoder_handler handler);

#endif //ARDUINO_AUTOPUMP_ENCODER_H
