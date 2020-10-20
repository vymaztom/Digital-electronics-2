/***********************************************************************
 *
 * Control LEDs using functions from GPIO and Timer libraries. Do not
 * use delay library any more.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2018-2020 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 *
 **********************************************************************/

/* Defines -----------------------------------------------------------*/
#define LED_D1  PB5
#define LED_D2  PB4
#define LED_D3  PB3
#define LED_D4  PB2
#define BUTTON  PD0

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "gpio.h"           // GPIO library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC

/* Function definitions ----------------------------------------------*/
/**
 * Main function where the program execution begins. Toggle three LEDs
 * on Multi-function shield with internal 8- and 16-bit timer modules.
 */


uint8_t move_value = 0;
uint8_t state_value = LED_D1;
uint8_t button_value = 0;

void move();

int main(void)
{
    /* Configuration of three LEDs */
    GPIO_config_output(&DDRB, LED_D1);
    GPIO_write_high(&PORTB, LED_D1);
	GPIO_config_output(&DDRB, LED_D2);
	GPIO_write_low(&PORTB, LED_D2);
	GPIO_config_output(&DDRB, LED_D3);
	GPIO_write_low(&PORTB, LED_D3);
	GPIO_config_output(&DDRB, LED_D4);
	GPIO_write_low(&PORTB, LED_D4);

	GPIO_config_input_pullup(&DDRD, BUTTON);

    /* Configuration of 16-bit Timer/Counter1
     * Set prescaler and enable overflow interrupt */
    TIM1_overflow_1s();
    TIM1_overflow_interrupt_enable();

	TIM0_overflow_16ms();
    TIM0_overflow_interrupt_enable();

    // Enables interrupts by setting the global interrupt mask
    sei();

    // Infinite loop
    while (1)
    {
        /* Empty loop. All subsequent operations are performed exclusively
         * inside interrupt service routines ISRs */


    }

    // Will never reach this
    return 0;
}

/* Interrupt service routines ----------------------------------------*/
/**
 * ISR starts when Timer/Counter1 overflows. Toggle LED D2 on
 * Multi-function shield. */

 ISR(TIMER0_OVF_vect)
 {
 	if(GPIO_read(&PIND, BUTTON) == 0)
 	{
 		move();
 	}
 }

ISR(TIMER1_OVF_vect)
{
	if(GPIO_read(&PIND, BUTTON) == 1)
	{
		move();
	}
}

void move()
{
	if((state_value == LED_D1)&&(move_value == 0))
	{
		move_value = 1;
		GPIO_write_low(&PORTB, LED_D1);
		GPIO_write_high(&PORTB, LED_D2);
		state_value = LED_D2;
	}
	else if((state_value == LED_D4)&&(move_value == 1))
	{
		move_value = 0;
		GPIO_write_low(&PORTB, LED_D4);
		GPIO_write_high(&PORTB, LED_D3);
		state_value = LED_D3;
	}
	else if(move_value == 1)
	{
		GPIO_write_low(&PORTB, state_value);
		state_value--;
		GPIO_write_high(&PORTB, state_value);
	}
	else
	{
		GPIO_write_low(&PORTB, state_value);
		state_value++;
		GPIO_write_high(&PORTB, state_value);
	}
}
