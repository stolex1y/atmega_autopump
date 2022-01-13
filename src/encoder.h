#ifndef ARDUINO_AUTOPUMP_ENCODER_H
#define ARDUINO_AUTOPUMP_ENCODER_H

#include <stdint.h>
#include <stdbool.h>

#include "pins.h"

#define CLICKS_DELAY_US (uint32_t) 200000
#define PRESS_DELAY_US 500

#define ENC_STATE_LENGTH 6

enum encoder_state {
    ENC_STATE_TURN_RIGHT,
    ENC_STATE_TURN_LEFT,
    ENC_STATE_CLICK,
    ENC_STATE_PRESS_N_TURN_LEFT,
    ENC_STATE_PRESS_N_TURN_RIGHT,
    ENC_STATE_DOUBLE_CLICK,
    ENC_STATE_UNDEFINED
};

struct encoder_pins {
    uint8_t s1;
    uint8_t s2;
    uint8_t key;
};

typedef void handler(void);

struct encoder {
    bool pressed;
    bool was_pressed;
    bool was_turned;
    uint8_t click_cnt;
    uint32_t last_click_us;
    uint32_t press_delay;

    uint8_t new_state;
    uint8_t old_state;

    handler* handlers[ENC_STATE_LENGTH];

    struct encoder_pins pins;
    enum encoder_state state;
};

struct encoder encoder_create(struct encoder_pins pins);
void encoder_init(struct encoder* encoder);
enum encoder_state encoder_get_curr_state(struct encoder* encoder);

#endif //ARDUINO_AUTOPUMP_ENCODER_H
