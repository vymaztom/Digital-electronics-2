#define LED_GREEN   PB5		// green LED
#define LED_RED     PC0		// red	 LED
#define BUTTON     	PD0
#define BLINK_DELAY 250

#ifndef F_CPU
#define F_CPU 16000000
#endif


#include <util/delay.h>
#include <avr/io.h>
#include "gpio.h"

int main(void)
{

    /* GREEN LED */
	GPIO_config_output(&DDRB, LED_GREEN);
	GPIO_write_low(&DDRB, LED_GREEN);

	/* RED LED */
	GPIO_config_output(&DDRC, LED_RED);
	GPIO_write_low(&DDRC, LED_RED);

	/* BUTTON */
	GPIO_config_input_pullup(&DDRD, BUTTON);

    while (1){

        _delay_ms(BLINK_DELAY);

		if(GPIO_read(&PIND, BUTTON) == 1)
		{
			GPIO_toggle(&PORTB, LED_GREEN);
			GPIO_toggle(&PORTC, LED_RED);
		}

    }

    return 0;
}
