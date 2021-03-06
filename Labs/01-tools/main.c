/***********************************************************************
 *
 * Blink a LED and use the function from the delay library.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2018-2020 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 *
 **********************************************************************/

#define LED_GREEN   PB5
#define SHORT_DELAY 100
#define DELAY 500
#define LONG_DELAY 1000

#ifndef F_CPU
#define F_CPU 16000000
#endif

#include <util/delay.h>
#include <avr/io.h>

int main(void){
    DDRB = DDRB | (1<<LED_GREEN);
    PORTB = PORTB & ~(1<<LED_GREEN);
    while (1){
		for(int i = 0 ; i < 3 ; i++){
			for(int j = 0 ; j < 3 ; j++){
				PORTB = PORTB ^ (1<<LED_GREEN);
				_delay_ms(SHORT_DELAY);
				if(i%2){
					_delay_ms(DELAY);
				}
				PORTB = PORTB ^ (1<<LED_GREEN);
				_delay_ms(DELAY);
			}
			_delay_ms(LONG_DELAY);
		}
		_delay_ms(LONG_DELAY);
    }
    return 0;
}
