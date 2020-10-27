/***********************************************************************
 *
 * Decimal counter with 7-segment output.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2018-2020 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 *
 **********************************************************************/

/* Includes ----------------------------------------------------------*/

#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include "segment.h"        // Seven-segment display library for AVR-GCC

/* Function definitions ----------------------------------------------*/
/**
 * Main function where the program execution begins. Display decimal
 * counter values on SSD (Seven-segment display) when 16-bit
 * Timer/Counter1 overflows.
 */
uint8_t value[2] = {0,0};

int main(void)
{
	// Configure SSD signals
	SEG_init();


	/* Configure 16-bit Timer/Counter1
	* Set prescaler and enable overflow interrupt */
	//TIM1_overflow_262ms();
	TIM1_overflow_1s();
	TIM1_overflow_interrupt_enable();

	/* Configure 8-bit Timer/Counter0
	* Set prescaler and enable overflow interrupt */
	TIM0_overflow_1024us();
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
* ISR starts when Timer/Counter0 overflows. Display decimal counter
*/
ISR(TIMER0_OVF_vect)
{
	static uint8_t dis = 0;
	dis++;
	if(dis > 1)
	{
		dis = 0;
	}
	SEG_update_shift_regs(value[dis],dis);
}

/* Interrupt service routines ----------------------------------------*/
/**
 * ISR starts when Timer/Counter1 overflows. Increment decimal counter
 * value and display it on SSD.
 */
ISR(TIMER1_OVF_vect)
{
	value[0] = value[0] + 1;
	if(value[0] > 9)
	{
		value[0] = 0;
		value[1] = value[1] + 1;
		if(value[1] > 5)
		{
			value[1] = 0;
		}
	}

}
