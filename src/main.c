#include <avr/interrupt.h>

#include <stdlib.h>
#include <stdint.h>

#include "lcd1602.h"
#include "encoder.h"
#include "autopump.h"
#include "uptime.h"
#include "plant.h"
#include "pins.h"

#define ENC_S1 8
#define ENC_S2 9
#define ENC_KEY 7

#define LCD_DATA_4 3
#define LCD_DATA_5 4
#define LCD_DATA_6 5
#define LCD_DATA_7 6
#define LCD_E 11
#define LCD_RS 12

#define PLANTS_LEN 3

static const char* plant_names[] = {
"Mint",
"Tomato",
"Potato"
};

static const uint8_t plant_pins[] = {
    PIN_A5,
    PIN_A4,
    PIN_A3
};

static void setup() {
    struct plant* plants = malloc(sizeof(struct plant) * PLANTS_LEN);
    for (size_t i = 0; i < PLANTS_LEN; i++) {
        plants[i] = plant_create(plant_names[i], plant_pins[i]);
    }

    const struct lcd1602_pins lcd_pins = (struct lcd1602_pins) {
        .DATA_4 = LCD_DATA_4,
        .DATA_5 = LCD_DATA_5,
        .DATA_6 = LCD_DATA_6,
        .DATA_7 = LCD_DATA_7,
        .E = LCD_E,
        .RS = LCD_RS
    };
    struct lcd1602* lcd = lcd1602_create(true, lcd_pins);

    const struct encoder_pins enc_pins = (struct encoder_pins) { .s1 = ENC_S1, .s2 = ENC_S2, .key = ENC_KEY };
    struct encoder* const encoder = encoder_create(enc_pins);

    autopump_init(lcd, encoder, plants, PLANTS_LEN);
    free(plants);
    sei();
}

static void loop() {
    autopump_timers_update();
}

int main() {
    uptime_init();
    setup();
	while(1) {
        loop();
	}
}