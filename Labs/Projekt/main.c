/****************************************************************************
 * Title:     Waveform generator with R-2R DAC
 * Author:    Tomas Vymazal <xvymaz11@vutbr.cz>  https://github.com/vymaztom
 * Software:  AVR-GCC 4.8
 * Target:    ATmega328
 *
 * DESCRIPTION
 *     Project witch demotrate generatiting of  DTFM, sine, square, triabgle
 *     and sawtooth signal from 200 to 2 KHz. Some kinds of signals supports
 *     seting of amplitude from 0 to 5 V devided into 255 steps.
 *
 *
 *****************************************************************************/

 /**
  * @mainpage
  * Waveform generator with R-2R DAC
  * @author Tomas Vymazal <xvymaz11@vutbr.cz>  https://github.com/vymaztom
  * @copyright (C) 2020 Tomas Vymazal, MIT License
  *
  * @file
  * @defgroup xvymaz waveform generator <main.c>
  * @code #include "FIFO.h" #include "TWI_LCD.h" #include "sinGeneratorEEPROM.h" @endcode
  *
  * @brief Project witch demotrate generatiting of  DTFM, sine, square, triabgle
  *     and sawtooth signal from 200 to 2 KHz.Some kinds of signals supports
  *     seting of amplitude from 0 to 5 V devided into 255 steps.
  *
  *
  * @author Tomas Vymazal xvymaz11@vutbr.cz  https://github.com/vymaztom
  *
  * @version   v1.0
  *
  * @copyright (C) 2020 Tomas Vymazal, MIT License
  *
  */



/******************************************************************************
frecvency generator - TABLE

			1209 Hz		1336 Hz		1477 Hz

697 HZ		  1			  2			  3

770 HZ		  4			  5			  6

852 Hz		  7			  8			  9

941 Hz		  *			  0			  #

******************************************************************************/

#ifndef F_CPU
#define F_CPU 16000000
#endif

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         			// AVR device-specific IO definitions
#include <avr/interrupt.h>  			// Interrupts standard C library for AVR-GCC
#include <avr/eeprom.h>					// EEPROM library for AVR-GCC
#include <stdlib.h>         			// C library. Needed for conversion function
#include <math.h>						// math library for AVR-GCC
#include "timer.h"						// Timer library for AVR-GCC
#include "gpio.h"						// GPIO library for AVR-GCC
#include "uart.h"						// Peter Fleury's UART librar
#include "twi.h"						// TWI/I2C library creted by Fryza
#include "FIFO.h"						// FIFI structure create by Vyma
#include "TWI_LCD.h"					// LCD connected by using TWI structure create by Vyma
#include "sinGeneratorEEPROM.h"			// SUPPORT lib of signal generator

/* Defines ----------------------------------------------------------*/

// define asembler function
extern void DAC(uint8_t current);

// define ADC port
#define DDRG DDRB
#define PORTG PORTB

// define frecvency values in HZ
#define f697 697
#define f770 770
#define f852 852
#define f941 941
#define f1209 1209
#define f1336 1336
#define f1477 1477

// define start address in EEPROM for generate Tables
#define DTFM_START_DATA_ADDR 255;

// define values for DTMF mode
#define TIME_OF_SAMPLE 0.000025
#define NUMBER_OF_SAMPLES 255
#define COEFICIENT_MULTIPLE_DEC 8
#define COEFICIENT_MULTIPLE_LOG2 3
#define START_DATA_ADDRES NULL
//#define TONE_LENGHT 4000 				// orogin to genarate singal of 100ms
#define TONE_LENGHT 40000
#define TONE_DISTANCE 1600
#define ADC_ZERO_VALUE 127




/* global variables ----------------------------------------------*/

button_t* b_matrix[4][3];
char b_matrix_names[4][3] = {{'1','2','3'},{'4','5','6'},{'7','8','9'},{'*','0','#'}};
freq_t* COLUMS[3];
freq_t* LINES[4];
button_t* g_next_button;
uint8_t period, devide, step;
uint8_t POINTER_G = 0;
uint8_t VOLATGE_G = 255;
uint16_t FREQ_G = 1000;
enum STATE_GENERATOR STATE_G = G_FREE;
uint8_t* START_DATA_ADDR = (uint8_t*)DTFM_START_DATA_ADDR;

/* Function definitions ----------------------------------------------*/


freq_t* create_freq(uint16_t frequency);
void print(const char* data);

/* Main function ----------------------------------------------*/
int main(){

	// Initialize UART to asynchronous, 8N1, 115200
	uart_init(UART_BAUD_SELECT(115200, F_CPU));



	// print to uart start state
	print("SELECT: DTFM MODE\r\n");
	//print("_________________\r\n");



	// Initialize TWI LCD
	twi_lcd_init(TWI_LCD_DISPLAY_ON);
	//twi_lcd_init(TWI_LCD_DISPLAY_ON | TWI_LCD_DISPLAY_CURSOR_ON | TWI_LCD_DISPLAY_BLINK_ON);

	twi_lcd_print_menu(G_DTFM, VOLATGE_G, FREQ_G, POINTER_G);

	// function for generation EEPROM data, uncomment if EEPROM is empty
	//generateSinIntoEEPROM(NUMBER_OF_SAMPLES, START_DATA_ADDRES);

	// config DAC port to output and set virtula null value
	DDRG |= 0xFF;
	PORTG |= ADC_ZERO_VALUE;

	// prepare variables for generating signal SINE, TRIABGLE, etc.
	period = MyRound((float)(1/TIME_OF_SAMPLE/FREQ_G));
	devide = MyRound((float)(NUMBER_OF_SAMPLES*TIME_OF_SAMPLE*FREQ_G*COEFICIENT_MULTIPLE_DEC));
	step = MyRound((float)255/period);


	// prepare arryes for create data matrix to define DTFM
	LINES[0] = create_freq(f697);
	LINES[1] = create_freq(f770);
	LINES[2] = create_freq(f852);
	LINES[3] = create_freq(f941);
	COLUMS[0] = create_freq(f1209);
	COLUMS[1] = create_freq(f1336);
	COLUMS[2] = create_freq(f1477);

	// prepare data matrix for generate DTFM
	for(uint8_t i = 0 ; i < 4 ; i++){
		for(uint8_t j = 0 ; j < 3 ; j++){
			button_t* one = (button_t*)malloc(sizeof(button_t));
			one->freq_1 = LINES[i];
			one->freq_2 = COLUMS[j];
			b_matrix[i][j] = one;
		}
	}


	// BLOCK for define KEYPAD input pins
	GPIO_config_input_pullup(&DDRC, PC0);
	GPIO_config_input_pullup(&DDRC, PC1);
	GPIO_config_input_pullup(&DDRC, PC2);
	GPIO_config_input_pullup(&DDRC, PC3);

	// BLOCK for define KEYPAD output pins
	GPIO_config_output(&DDRD, PD4);
	GPIO_config_output(&DDRD, PD5);
	GPIO_config_output(&DDRD, PD6);
	GPIO_config_output(&DDRD, PD7);

	// BLOCK for define KEYPAD output pins to null
	GPIO_write_low(&PORTD, PD4);
	GPIO_write_low(&PORTD, PD5);
	GPIO_write_low(&PORTD, PD6);
	GPIO_write_low(&PORTD, PD7);


	GPIO_config_output(&DDRC, PC6);


	// Configure 16-bit Timer/Counter1
	// Configure timer 1 for CTC mode
    TCCR1B |= (1 << WGM12 );
	// Set CTC compare value 40KHz (25 us)
    OCR1A = 400;
	// Enable CTC interrupt
	TIMSK1 |= (1 << OCIE1A );
	// set null devider of register = TIM1_overflow_4ms()
	TCCR1B &= ~((1<<CS12) | (1<<CS11));
	TCCR1B |= (1<<CS10);


    // Enables interrupts by setting the global interrupt mask
    sei();

    // Infinite loop
    while (1){
		GPIO_toggle(&PORTC, PC6);

		 for(uint8_t i = 0 ; i < 4 ; i++){
			 GPIO_write_high(&PORTD, PD4+i);
			 _delay_us(10);
			 uint8_t value = PINC & 0xF;
			 if((value < 4) && (value > 0)){
				 FIFO_add((void*)b_matrix[i][value-1]);
				 uart_puts("Pressed button: ");
				 uart_putc(b_matrix_names[i][value-1]);
				 uart_puts("\r\n");
				 _delay_ms(500);
			 }
			 if(value == 4){
				 if(i == 0){
					 STATE_G = G_FREE;
					 twi_lcd_print_menu(G_DTFM, VOLATGE_G, FREQ_G, POINTER_G);
					 uart_puts("SELECT: DTFM MODE\r\n");
					 /*
					 uart_puts("-----------------\r\n");
					 */
				 }else if(i == 1){
					 STATE_G = G_SQUARE;
					 twi_lcd_print_menu(G_SQUARE, VOLATGE_G, FREQ_G, POINTER_G);
					 uart_puts("SELECT: SQUARE MODE\r\n");
					 /*
					 uart_puts("-------------------\r\n");
					 */
				 }else if(i == 2){
					 STATE_G = G_SAWTOOTH;
					 twi_lcd_print_menu(G_SAWTOOTH, VOLATGE_G, FREQ_G, POINTER_G);
					 uart_puts("SELECT: SAWTOOTH MODE\r\n");
					 /*
					 uart_puts("---------------------\r\n");
					 */
				 }else if(i == 3){
					 if(POINTER_G){
						 if (FREQ_G > 200){
							 FREQ_G--;
						 }
						 period = MyRound((float)(1/TIME_OF_SAMPLE/FREQ_G));
						 devide = MyRound((float)(NUMBER_OF_SAMPLES*TIME_OF_SAMPLE*FREQ_G*COEFICIENT_MULTIPLE_DEC));
						 step = MyRound((float)255/period);
						 twi_lcd_print_freq(FREQ_G, POINTER_G);
					 }else{
						 if(VOLATGE_G > 0){
							 VOLATGE_G--;
						 }
						 period = MyRound((float)(1/TIME_OF_SAMPLE/FREQ_G));
						 devide = MyRound((float)(NUMBER_OF_SAMPLES*TIME_OF_SAMPLE*FREQ_G*COEFICIENT_MULTIPLE_DEC));
						 step = MyRound((float)255/period);
						 twi_lcd_print_voltage(VOLATGE_G, POINTER_G);
					 }
				 }
			 }
			 if(value == 5){
				 if(i == 0){
					 STATE_G = G_SINE;
					 twi_lcd_print_menu(G_SINE, VOLATGE_G, FREQ_G, POINTER_G);
					 uart_puts("SELECT: SINE MODE\r\n");
					 /*
					 uart_puts("-----------------\r\n");
					 */
				 }else if(i == 1){
					 STATE_G = G_TRIABGLE;
					 twi_lcd_print_menu(G_TRIABGLE, VOLATGE_G, FREQ_G, POINTER_G);
					 uart_puts("SELECT: TRIABGLE MODE\r\n");
					 /*
					 uart_puts("---------------------\r\n");
					 */
				 }else if(i == 2){
					 if(POINTER_G){
						 POINTER_G = 0;
					 }else{
						 POINTER_G = 1;
					 }
					 twi_lcd_print_freq(FREQ_G, POINTER_G);
					 twi_lcd_print_voltage(VOLATGE_G, POINTER_G);
				 }else if(i == 3){
					 if(POINTER_G){
						 if (FREQ_G < 2000){
							 FREQ_G++;
						 }
						 period = MyRound((float)(1/TIME_OF_SAMPLE/FREQ_G));
						 devide = MyRound((float)(NUMBER_OF_SAMPLES*TIME_OF_SAMPLE*FREQ_G*COEFICIENT_MULTIPLE_DEC));
						 twi_lcd_print_freq(FREQ_G, POINTER_G);
					 }else{
						 if(VOLATGE_G < 255){
							 VOLATGE_G++;
						 }
						 period = MyRound((float)(1/TIME_OF_SAMPLE/FREQ_G));
						 devide = MyRound((float)(NUMBER_OF_SAMPLES*TIME_OF_SAMPLE*FREQ_G*COEFICIENT_MULTIPLE_DEC));
						 twi_lcd_print_voltage(VOLATGE_G, POINTER_G);
					 }
				 }
			 }
			 GPIO_write_low(&PORTD, PD4+i);
		 }
    }

    // end of program
    return 0;
}


 /**
* @brief TIMER1_COMPA_vect set to 40 kHz
* @details ISR rutine run
*/
ISR(TIMER1_COMPA_vect, ISR_BLOCK){
	static uint16_t c_time_pause = 0;
	static uint16_t c_time_freq = 0;
	static uint8_t is_null = 0;
	button_t* get = NULL;
	c_time_pause++;
	switch(STATE_G){
		case G_DTFM:
			if(c_time_pause > TONE_LENGHT){
				c_time_pause = 0;
				DAC_FAST(ADC_ZERO_VALUE);
				STATE_G = G_PAUSE;
				break;
			}

			++g_next_button->freq_1->c_time;
			if(g_next_button->freq_1->c_time >= g_next_button->freq_1->c_period){
				g_next_button->freq_1->c_time = 0;
			}

			++g_next_button->freq_2->c_time;
			if(g_next_button->freq_2->c_time >= g_next_button->freq_2->c_period){
				g_next_button->freq_2->c_time = 0;
			}

			if(eeprom_is_ready()){
				uint8_t value1 = eeprom_read_byte((uint8_t*)((g_next_button->freq_1->c_time + g_next_button->freq_1->data_address)));
				uint8_t value2 = eeprom_read_byte((uint8_t*)((g_next_button->freq_2->c_time + g_next_button->freq_2->data_address)));
				uint16_t DAC_value = value1 + value2;
				DAC_FAST((uint8_t)(DAC_value >> 1));
			}
			break;

		case G_SINE:
			++c_time_freq;
			if(c_time_freq >= period){
				c_time_freq = 0;
			}
			if(eeprom_is_ready()){
				uint8_t value = eeprom_read_byte((uint8_t*)((c_time_freq*devide) >> COEFICIENT_MULTIPLE_LOG2));
				DAC((value*VOLATGE_G) >> 8);
			}
			break;
		case G_SQUARE:
			++c_time_freq;
			if(c_time_freq >= (period >> 1)){
				c_time_freq = 0;
				if(is_null){
					DAC(VOLATGE_G);
					is_null = 0;
				}else{
					DAC(0);
					is_null = 1;
				}
			}
			break;
		case G_TRIABGLE:
			++c_time_freq;
			if(c_time_freq >= (period >> 1)){
				c_time_freq = 0;
				if(is_null){
					is_null = 0;
				}else{
					is_null = 1;
				}
			}
			if(is_null){
				DAC((step*VOLATGE_G*c_time_freq) >> 7);
			}else{
				DAC(VOLATGE_G - ((step*VOLATGE_G*c_time_freq) >> 7));
			}
			break;
		case G_SAWTOOTH:
			++c_time_freq;
			if(c_time_freq >= period){
				c_time_freq = 0;
			}
			DAC((step*VOLATGE_G*c_time_freq) >> 8);
			break;
		case G_PAUSE:
			if(c_time_pause > TONE_DISTANCE){
				c_time_pause = 0;
				STATE_G = G_FREE;
				break;
			}
			break;
		case G_FREE:
			get = (button_t*)FIFO_get();
			c_time_pause = 0;
			DAC_FAST(ADC_ZERO_VALUE);
			if(get != NULL){
				g_next_button = get;
				STATE_G = G_DTFM;
			}
			break;
	}


}


/**
* @brief functin for creating instance of freq_t
* @param [uint16_t] <frequency> <parameter_description>
* @return <return_description>
* @details <details>
*/
freq_t* create_freq(uint16_t frequency){
	freq_t* one = (freq_t*)malloc(sizeof(freq_t));
	if(one != NULL){
		one->c_time = 0;
		one->c_period = MyRound((float)(1/TIME_OF_SAMPLE/frequency));
		one->c_devider = MyRound((float)(NUMBER_OF_SAMPLES*TIME_OF_SAMPLE*frequency*COEFICIENT_MULTIPLE_DEC));
		one->frequency = frequency;
		one->data_address = START_DATA_ADDR;
		START_DATA_ADDR += one->c_period;
		for(uint8_t i = 0 ; i < one->c_period; i++){
			eeprom_write_byte((uint8_t*)(one->data_address+i), MyRound(127 + 128*sin(TWO_PI/(float)one->c_period*i)));
		}
	}
	return one;
}


// f*cked uart_puts from Peter`s Fleury lib not work !!!!!!!!!!!!!!!!
void print(const char* data){
	uint8_t i = 0;
	while(data[i] != '\0'){
		uart_putc(data[i]);
		i++;
	}
}
