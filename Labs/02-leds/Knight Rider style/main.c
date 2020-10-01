#define LEDS   5
#define BUTTON     	PD0
#define BLINK_DELAY 10
#define POT 5

#ifndef F_CPU
#define F_CPU 16000000
#endif


#include <util/delay.h>     // Functions for busy-wait delay loops
#include <avr/io.h>         // AVR device-specific IO definitions


uint16_t ADCsingleREAD(uint8_t adctouse);

int main(void){

	uint8_t reg = 0;
	for(uint8_t i = 0 ; i < LEDS ; ++i){
		reg <<= 1;
		reg++;
	}

    DDRC = DDRC | reg;
    PORTC = PORTC & ~(reg);
	PORTC = PORTC | (1<<((LEDS/2)+(LEDS%2)));

	/* BUTTON */
	DDRD = DDRD & ~(1<<BUTTON);
	PORTD = PORTD | (1<<BUTTON);

    // Infinite loop
    while (LEDS > 0){

		// Pause several milliseconds
		uint16_t value = ADCsingleREAD(POT) >> 4;
		for(uint16_t i = 0 ; i < value ; ++i){
			_delay_ms(BLINK_DELAY);
		}
		if(bit_is_clear(PIND, 0)){
			if(PORTC == (1<<LEDS)){
				PORTC = 1;
			}else{
				PORTC <<= 1;
			}
		}else{
			if(PORTC == 1){
				PORTC = (1<<LEDS);
			}else{
				PORTC >>= 1;
			}
		}

    }

    return 0;
}

uint16_t ADCsingleREAD(uint8_t adctouse){
    uint16_t ADCval = 0;

    ADMUX = adctouse;
    ADMUX |= (1 << REFS0);
    ADMUX &= ~(1 << ADLAR);

    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    ADCSRA |= (1 << ADEN);

    ADCSRA |= (1 << ADSC);
    while(ADCSRA & (1 << ADSC));

    ADCval = (uint16_t)ADCL;
    ADCval = (uint16_t)((ADCH << 8) + ADCval);

    return ADCval;
}
