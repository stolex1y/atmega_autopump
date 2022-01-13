#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdlib.h>
#include <stdio.h>

#include "gpio.h"
#include "timer.h"
#include "lcd1602.h"
#include "eeprom.h"
#include "encoder.h"
#include "autopump.h"

#define LED_PIN A5

#define ENC_S1 8
#define ENC_S2 9
#define ENC_KEY 7

#define LCD_DATA_4 3
#define LCD_DATA_5 4
#define LCD_DATA_6 5
#define LCD_DATA_7 6
#define LCD_E 11
#define LCD_RS 12

static struct lcd1602* lcd = &(struct lcd1602) {
        .is_4bit = true,
        .DATA_4 = LCD_DATA_4,
        .DATA_5 = LCD_DATA_5,
        .DATA_6 = LCD_DATA_6,
        .DATA_7 = LCD_DATA_7,
        .E = LCD_E,
        .RS = LCD_RS
};

static size_t plants_len = 3;
static char* plants[] = {
        "Mint",
        "Tomato",
        "Potato"
};

static void click() {
    lcd1602_clear(lcd);
    lcd1602_to_start_pos(lcd);
    lcd1602_send_string(lcd, "Clicked");
}

static void double_click() {
    lcd1602_clear(lcd);
    lcd1602_to_start_pos(lcd);
    lcd1602_send_string(lcd, "Double clicked");
}

static void turn_right() {
    lcd1602_clear(lcd);
    lcd1602_to_start_pos(lcd);
    lcd1602_send_string(lcd, "Turned right");
}

static void turn_left() {
    lcd1602_clear(lcd);
    lcd1602_to_start_pos(lcd);
    lcd1602_send_string(lcd, "Turned left");
}

static void press_n_turn_right() {
    lcd1602_clear(lcd);
    lcd1602_to_start_pos(lcd);
    lcd1602_send_string(lcd, "Pressed and turned right");
}

static void press_n_turn_left() {
    lcd1602_clear(lcd);
    lcd1602_to_start_pos(lcd);
    lcd1602_send_string(lcd, "Pressed and turned left");
}

static void setup() {
    pin_mode(LED_PIN, OUTPUT);
    digital_write(LED_PIN, HIGH);
    _delay_ms(500);
    digital_write(LED_PIN, LOW);


    lcd1602_init(lcd);
    lcd1602_clear(lcd);
    lcd1602_to_start_pos(lcd);
//    lcd1602_send_string(lcd, "Hello, try to use the encoder");

    struct encoder_pins pins = (struct encoder_pins) { .s1 = ENC_S1, .s2 = ENC_S2, .key = ENC_KEY };
    struct encoder encoder = encoder_create(pins);

//    encoder.handlers[ENC_STATE_CLICK] = click;
//    encoder.handlers[ENC_STATE_DOUBLE_CLICK] = double_click;
//    encoder.handlers[ENC_STATE_PRESS_N_TURN_LEFT] = press_n_turn_left;
//    encoder.handlers[ENC_STATE_PRESS_N_TURN_RIGHT] = press_n_turn_right;
//    encoder.handlers[ENC_STATE_TURN_LEFT] = turn_left;
//    encoder.handlers[ENC_STATE_TURN_RIGHT] = turn_right;


    sei();

    encoder_init(&encoder);
    autopump_init(lcd, &encoder, plants, plants_len);
}

static void loop() {

}

int main() {
    setup();
	while(1) {
        loop();
	}
}