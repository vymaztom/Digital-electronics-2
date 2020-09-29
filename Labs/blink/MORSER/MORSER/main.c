/*
 * MORSER.c
 *
 * Created: 23.09.2020 13:27:28
 * Author : vyma9
 */ 
#define LED_GREEN   PB5
#define SHORT_DELAY 500
#ifndef F_CPU
#define F_CPU 16000000      // CPU frequency in Hz required for delay func
#endif


#include <avr/io.h>
#include <util/delay.h>




int main(void)
{
    // Set pin as output in Data Direction Register
    // DDRB = DDRB or 0010 0000
    DDRB = DDRB | (1<<LED_GREEN);

    // Set pin LOW in Data Register (LED off)
    // PORTB = PORTB and 1101 1111
    PORTB = PORTB & ~(1<<LED_GREEN);

    // Infinite loop
    while (1){
	    // Pause several miliseconds
	    _delay_ms(SHORT_DELAY);

	    // Invert LED in Data Register
	    // PORTB = PORTB xor 0010 0000
	    PORTB = PORTB ^ (1<<LED_GREEN);
    }

    return 0;
}

