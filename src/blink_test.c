#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>



int main(void) {
	DDRB = 0x10;
	PORTB = 0xFF;
	DDRC = 0x10;
	PORTC = 0xFF;
	//PORTС &= ~1<<5;
	while(1) {
		/*PORTС |= 1<<5;
		_delay_ms(500);
		PORTС &= ~1<<5; 
		_delay_ms(500);*/
	}
}