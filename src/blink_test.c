#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "gpio.h"
#include "timer.h"
#include "lcd1602.h"

#define LED1_PIN 5
#define LED2_PIN 13

//uint8_t val;

/*static void timer1_vector() {
    digital_toggle(LED1_PIN);
}*/

static void setup() {
    pin_mode(LED2_PIN, OUTPUT);
//    pin_mode(LED1_PIN, OUTPUT);
//    timer1_init(1, timer1_vector);
//    sei();
    lcd1602_init();
    lcd1602_set_cursor_pos(0, 0);
    lcd1602_send_string("by Alexey");
    lcd1602_set_cursor_pos(10, 1);
    lcd1602_send_string("Filimonov");
}

static void loop() {
//    val = digital_read(LED1_PIN);
//    digital_write(LED2_PIN, val);
}

int main() {
    setup();
	while(1) {
        loop();
	}
}