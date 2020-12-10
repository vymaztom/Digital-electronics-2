/***********************************************************************
 *
 * The I2C bus scanner detects the addresses of the modules that are
 * connected to the SDA and SCL signals. A simple description of FSM is
 * used.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2017-2020 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 *
 **********************************************************************/

/* Defines -----------------------------------------------------------*/

#ifndef F_CPU
#define F_CPU 16000000
#endif

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <stdlib.h>         // C library. Needed for conversion function
#include "uart.h"           // Peter Fleury's UART library
#include "twi.h"            // TWI library for AVR-GCC
#include <util/delay.h>
#include "TWI_LCD.h"

void uart_printnumber(uint16_t value){
	char lcd_string[9] = "000000000";
	itoa(value, lcd_string, 2);
	uart_puts(lcd_string);
	return;
}

void ToBin(char in){
	uart_putc('0');
	uart_putc('x');
	for(uint8_t i = 8 ; i > 0 ; i--){
		if(i == 4){
			uart_putc(' ');
		}
		if((in >> i-1) & 1){
			uart_putc('1');
		}else{
			uart_putc('0');
		}
	}
}


/* Variables ---------------------------------------------------------*/
typedef enum {              // FSM declaration
    STATE_IDLE = 1,
    STATE_SEND,
    STATE_ACK
} state_t;

/* Function definitions ----------------------------------------------*/
/**
 * Main function where the program execution begins. Use Timer/Counter1
 * and send I2C (TWI) address every 33 ms. Send information about
 * scanning process to UART.
 */

uint8_t customChar[8] = {
	 0b00100,
	 0b01110,
	 0b00100,
	 0b01110,
	 0b10101,
	 0b00100,
	 0b01010,
	 0b01010
};
int main(void)
{
    // Initialize I2C (TWI)
    twi_lcd_init(TWI_LCD_DISPLAY_ON | TWI_LCD_DISPLAY_CURSOR_ON | TWI_LCD_DISPLAY_BLINK_ON);
	twi_lcd_set_lines(TWI_LCD_FUNCTION_4BIT_2LINES);

    // Initialize UART to asynchronous, 8N1, 9600
    uart_init(UART_BAUD_SELECT(9600, F_CPU));

	 ToBin('0');
	 uart_putc('\n');
	 uart_printnumber((uint8_t)'0');
	 uart_putc(0b00001100);

    // Configure 16-bit Timer/Counter1 to update FSM
    // Enable interrupt and set the overflow prescaler to 33 ms
    //TIM1_overflow_33ms();
    //TIM1_overflow_interrupt_enable();

    // Enables interrupts by setting the global interrupt mask
    sei();

    // Put strings to ringbuffer for transmitting via UART
    uart_puts("\r\nScan I2C-bus for devices:\r\n");
	//twi_lcd_send(0b01000100, TWI_LCD_DATA);
	//send(0b00000010);
	//send(0);
	//send(0b00001111);
	//send(0b00100100);
	//send(0b00100100);
	//twi_lcd_printChar('D');
	twi_lcd_data(0, customChar);
	twi_lcd_printChar(0);
	twi_lcd_print("DE");
	twi_lcd_print("DE2");
	twi_lcd_print("DE3");
	twi_lcd_gotoxy(0, 1);
	twi_lcd_print("!@#$%s^&*()");
	twi_lcd_clean();
	twi_lcd_gotoxy(0, 0);





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
 * ISR starts when Timer/Counter1 overflows. Update Finite State Machine
 * and test I2C slave addresses between 8 and 119.
 */
/*
ISR(TIMER1_OVF_vect)
{
    static state_t state = STATE_IDLE;  // Current state of the FSM
    static uint8_t addr = 7;            // I2C slave address
    uint8_t result = 1;                 // ACK result from the bus
    char uart_string[2] = "00"; // String for converting numbers by itoa()

    // FSM
    switch (state)
    {
    // Increment I2C slave address
    case STATE_IDLE:
        addr++;
        // If slave address is between 8 and 119 then move to SEND state
		if((addr > 7)&&(addr < 120)){
			state = STATE_SEND;
		}
        break;

    // Transmit I2C slave address and get result
    case STATE_SEND:
        // I2C address frame:
        // +------------------------+------------+
        // |      from Master       | from Slave |
        // +------------------------+------------+
        // | 7  6  5  4  3  2  1  0 |     ACK    |
        // |a6 a5 a4 a3 a2 a1 a0 R/W|   result   |
        // +------------------------+------------+
        result = twi_start((addr<<1) + TWI_WRITE);
        twi_stop();
		*/
        /* Test result from I2C bus. If it is 0 then move to ACK state,
         * otherwise move to IDLE */
		 /*
		 if(result){
			 state = STATE_IDLE;
		 }else{
			 state = STATE_ACK;
		 }

        break;

    // A module connected to the bus was found
    case STATE_ACK:
        // Send info about active I2C slave to UART and move to IDLE
		uart_puts("addr: ");
		uart_printnumber(addr);
		uart_puts("\r\n");
		state = STATE_IDLE;
        break;

    // If something unexpected happens then move to IDLE
    default:
        state = STATE_IDLE;
        break;
    }
}*/
