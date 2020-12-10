/**
* @file main.c
* @author Tomáš Vymazal
* @date 1 Dec 2020
* @copyright 2020 Tomáš Vymazal
* @brief <brief>
*/

/******************************************************************************
frecvency generator - TABLE

			1209 Hz		1336 Hz		1477 Hz

697 HZ		  1			  2			  3

770 HZ		  4			  5			  6

852 Hz		  7			  8			  9

941 Hz		  *			  0			  #

-------------------------------------------------------------------------------

f1 = 697 Hz 	-> a = 1434.72023 us	-> cycele c =
f2 = 770 Hz 	-> a = 1298.70129 us
f3 = 852 Hz 	-> a = 1173.70892 us
f4 = 941 Hz 	-> a = 1062.69926 us
f5 = 1209 Hz 	-> a = 827.12986 us
f6 = 1336 Hz 	-> a = 748.50299 us
f7 = 1477 Hz 	-> a = 677.04807 us

******************************************************************************/

#ifndef F_CPU
#define F_CPU 16000000
#endif

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <avr/eeprom.h>
#include <stdlib.h>         // C library. Needed for conversion function
#include <math.h>
#include "timer.h"
#include "gpio.h"          // Timer library for AVR-GCC
#include "lcd.h"            // Peter Fleury's LCD library
#include "uart.h"           // Peter Fleury's UART librar
#include "twi.h"
#include "FIFO.h"
#include "TWI_LCD.h"
#include "sinGeneratorEEPROM.h"

/* Defines ----------------------------------------------------------*/



#define Dev24C02  0x20

#define f697 697
#define f770 770
#define f852 852
#define f941 941
#define f1209 1209
#define f1336 1336
#define f1477 1477

#define DTFM_START_DATA_ADDR 255;

#define TIME_OF_SAMPLE 0.000025
#define NUMBER_OF_SAMPLES 255
#define COEFICIENT_MULTIPLE_DEC 8
#define COEFICIENT_MULTIPLE_LOG2 3
#define START_DATA_ADDRES NULL
#define TONE_LENGHT 6250
#define TONE_DISTANCE 3125
#define ADC_ZERO_VALUE 127


typedef struct freq{
	uint8_t c_time;
	uint8_t c_period;
	uint8_t c_devider;
	uint16_t frequency;
	uint8_t* data_address;
}freq_t;

enum STATE_GENERATOR{
	G_DTFM,
	G_SINE,
	G_SQUARE,
	G_TRIABGLE,
	G_SAWTOOTH,
	G_PAUSE,
	G_FREE
};

typedef struct button{
	freq_t* freq_1;
	freq_t* freq_2;
	char ch_value;
	enum STATE_GENERATOR waveform;
	uint32_t wave_lenght;
	uint32_t wave_distance;
	uint8_t min;
	uint8_t max;
}button_t;

button_t b1, b2;
button_t* b_matrix[4][3];
freq_t* COLUMS[3];
freq_t* LINES[4];
button_t* g_next_button;
uint8_t POINTER_G = 0;
uint8_t VOLATGE_G = 255;
uint16_t FREQ_G = 1000;
uint8_t period, devide, step;
enum STATE_GENERATOR STATE_G = G_DTFM;
uint8_t* START_DATA_ADDR = DTFM_START_DATA_ADDR;
/* Function definitions ----------------------------------------------*/



void twi_lcd_print_menu(enum STATE_GENERATOR kind){
	twi_lcd_clean();
	twi_lcd_gotoxy(0, 0);
	switch (kind){
		case G_DTFM:
		twi_lcd_print("DTFM MODE");
		break;

		case G_SINE:
		twi_lcd_print("SINE");
		twi_lcd_print_voltage(VOLATGE_G, POINTER_G);
		twi_lcd_print_freq(FREQ_G, POINTER_G);
		break;

		case G_SQUARE:
		twi_lcd_print("SQUE");
		twi_lcd_print_voltage(VOLATGE_G, POINTER_G);
		twi_lcd_print_freq(FREQ_G, POINTER_G);
		break;

		case G_TRIABGLE:
		twi_lcd_print("TRIA");
		twi_lcd_print_voltage(VOLATGE_G, POINTER_G);
		twi_lcd_print_freq(FREQ_G, POINTER_G);
		break;

		case G_SAWTOOTH:
		twi_lcd_print("SAWT");
		twi_lcd_print_voltage(VOLATGE_G, POINTER_G);
		twi_lcd_print_freq(FREQ_G, POINTER_G);
		break;
	}
}

void twi_lcd_print_voltage(uint8_t value, uint8_t pointer){
	twi_lcd_gotoxy(5, 0);
	if(pointer == 0){
		twi_lcd_print("*");
	}else{
		twi_lcd_print(" ");
	}
	twi_lcd_print("U= ");
	uint16_t value_long = (uint16_t)value;
	uint8_t volt[3] = {0,0,0};
	for(uint8_t i = 0 ; i < 3 ; i++){
		while(value_long > 51){
			value_long -= 51;
			volt[i]++;
		}
		value_long *= 10;
	}
	twi_lcd_print_number(volt[0]);
	twi_lcd_print(".");
	twi_lcd_print_number(volt[1]);
	twi_lcd_print_number(volt[2]);
	twi_lcd_gotoxy(13, 0);
	twi_lcd_print(" V");
}

void twi_lcd_print_freq(uint16_t value, uint8_t pointer){
	twi_lcd_gotoxy(5, 1);
	if(pointer == 1){
		twi_lcd_print("*");
	}else{
		twi_lcd_print(" ");
	}
	twi_lcd_print("f= ");
	twi_lcd_print_number(value);
	twi_lcd_gotoxy(13, 1);
	twi_lcd_print(" Hz");
}

/**
* @brief <brief>
* @param [in] <name> <parameter_description>
* @return <return_description>
* @details <details>
*/
void uart_printnumber(uint16_t value){
	char lcd_string[5] = "00000";
	itoa(value, lcd_string, 10);
	uart_puts(lcd_string);
	uart_puts("\r\n");
	return;
}

void print_freq_data(freq_t* one){
	char lcd_string[5] = "00000";
	uart_puts("freq: ");
	itoa(one->frequency, lcd_string, 10);
	uart_puts(lcd_string);
	uart_puts(" C_period: ");
	itoa(one->c_period, lcd_string, 10);
	uart_puts(lcd_string);
	uart_puts(" C_devider: ");
	itoa(one->c_devider, lcd_string, 10);
	uart_puts(lcd_string);
	uart_puts("\r\n");
	uart_puts("----------------------------\r\n");
}


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

uint8_t GLOBAL = 0;

int main(){


	// Initialize I2C (TWI)


	// Initialize UART to asynchronous, 8N1, 9600
	uart_init(UART_BAUD_SELECT(115200, F_CPU));

	uart_puts("START\r\n");

	twi_lcd_init(TWI_LCD_DISPLAY_ON | TWI_LCD_DISPLAY_CURSOR_ON | TWI_LCD_DISPLAY_BLINK_ON);
	twi_lcd_set_lines(TWI_LCD_FUNCTION_4BIT_2LINES);
	/*
	twi_lcd_print("DE");
	twi_lcd_print("DE2");
	twi_lcd_print("DE3");
	twi_lcd_gotoxy(0, 1);
	twi_lcd_print("!@#$%s^&*()");
	twi_lcd_gotoxy(0, 0);
	twi_lcd_clean();
	*/
	twi_lcd_print_menu(G_DTFM);


	//generateSinIntoEEPROM(NUMBER_OF_SAMPLES, START_DATA_ADDRES);

	DDRB |= 0xFF;
	//DDRC |= 0xf;
	// ...and turn LED off in Data Register
	PORTB |= 0x0F;
	//PORTC |= 0xf;

	period = MyRound((float)(1/TIME_OF_SAMPLE/FREQ_G));
	devide = MyRound((float)(NUMBER_OF_SAMPLES*TIME_OF_SAMPLE*FREQ_G*COEFICIENT_MULTIPLE_DEC));
	step = MyRound((float)255/period);

	LINES[0] = create_freq(f697);
	LINES[1] = create_freq(f770);
	LINES[2] = create_freq(f852);
	LINES[3] = create_freq(f941);
	COLUMS[0] = create_freq(f1209);
	COLUMS[1] = create_freq(f1336);
	COLUMS[2] = create_freq(f1477);

	for(uint8_t i = 0 ; i < 4 ; i++){
		for(uint8_t j = 0 ; j < 3 ; j++){
			button_t* one = (button_t*)malloc(sizeof(button_t));
			one->freq_1 = LINES[i];
			one->freq_2 = COLUMS[j];
			one->ch_value = '1';
			one->waveform = G_DTFM;
			one->wave_lenght = TONE_LENGHT;
			one->wave_distance = TONE_DISTANCE;
			b_matrix[i][j] = one;
		}
	}



	b1.freq_1 = LINES[0];
	b1.freq_2 = COLUMS[0];
	b1.ch_value = '1';
	b1.waveform = G_DTFM;
	b1.wave_lenght = TONE_LENGHT;
	b1.wave_distance = TONE_DISTANCE;

	b2.freq_1 = LINES[0];
	b2.freq_2 = COLUMS[0];
	b2.ch_value = '1';
	b2.waveform = G_SINE;
	b2.wave_lenght = TONE_LENGHT;
	b2.wave_distance = TONE_DISTANCE;


	uart_printnumber(START_DATA_ADDR);

	FIFO_init();
	if(FIFO_add((void*)&b1)){
		uart_puts("add b1\r\n");
	}
	if(FIFO_add((void*)&b2)){
		uart_puts("add b2\r\n");
	}
	g_next_button = (button_t*)FIFO_get();
	STATE_G = g_next_button->waveform;
	if(g_next_button == NULL){
		uart_puts("ret == NULL\r\n");
	}


	GPIO_config_output(&DDRC, PC6);
	GPIO_write_low(&PORTC, PC6);
	GPIO_config_output(&DDRD, PD3);
	GPIO_write_low(&PORTD, PD3);


	GPIO_config_input_pullup(&DDRC, PC0);
	GPIO_config_input_pullup(&DDRC, PC1);
	GPIO_config_input_pullup(&DDRC, PC2);
	GPIO_config_input_pullup(&DDRC, PC3);

	GPIO_config_output(&DDRD, PD4);
	GPIO_config_output(&DDRD, PD5);
	GPIO_config_output(&DDRD, PD6);
	GPIO_config_output(&DDRD, PD7);

	GPIO_write_low(&PORTD, PD4);
	GPIO_write_low(&PORTD, PD5);
	GPIO_write_low(&PORTD, PD6);
	GPIO_write_low(&PORTD, PD7);










	/*
	GPIO_config_output(&DDRC, PC1);
	GPIO_write_low(&PORTC, PC1);
	*/

	// Configure 16-bit Timer/Counter1
    // Enable interrupt and set the overflow prescaler to 262 ms
	//TIM0_overflow_128us();
	//TIM0_overflow_16us();
	//TIM0_overflow_interrupt_enable();

    // Set up Timer/Counter1
    TCCR1B |= (1 << WGM12 );   // Configure timer 1 for CTC mode
    OCR1A = 400;             // Set CTC compare value to 10Hz (100mS)
                               // at 16MHz AVR clock, with a prescaler of 64
    TIMSK1 |= (1 << OCIE1A );  // Enable CTC interrupt
    //TCCR1B |= ((1 << CS10 ) | (1 << CS11 )); // Start Timer/Counter1 at F_CPU/64
	TCCR1B &= ~((1<<CS12) | (1<<CS11)); TCCR1B |= (1<<CS10);

    // Enable all interrupts
    //sei();

	//TIM2_overflow_16ms();
	//TIM2_overflow_interrupt_enable();





    // Enables interrupts by setting the global interrupt mask
    sei();


	print_freq_data(LINES[0]);
	print_freq_data(LINES[1]);
	print_freq_data(LINES[2]);
	print_freq_data(LINES[3]);
	print_freq_data(COLUMS[0]);
	print_freq_data(COLUMS[1]);
	print_freq_data(COLUMS[2]);


	//twi_lcd_clean();
	//twi_lcd_gotoxy(0, 0);


    // Infinite loop
    while (1){
        /* Empty loop. All subsequent operations are performed exclusively
         * inside interrupt service routines ISRs */


		 GPIO_toggle(&PORTC, PC6);
		 for(uint8_t i = 0 ; i < 4 ; i++){
			 GPIO_write_high(&PORTD, PD4+i);
			 _delay_us(10);
			 uint8_t value = PINC & 0xF;
			 GLOBAL = value;
			 if((value < 4) && (value > 0)){
				 FIFO_add((void*)b_matrix[i][value-1]);
				 uart_puts("DATA\r\n");
			 }
			 if(value == 4){
				 uart_puts("COLLUM: 4\r\n");
				 if(i == 0){
					 STATE_G = G_FREE;
					 twi_lcd_print_menu(G_DTFM);
				 }else if(i == 1){
					 STATE_G = G_SQUARE;
					 twi_lcd_print_menu(G_SQUARE);
				 }else if(i == 2){
					 STATE_G = G_SAWTOOTH;
					 twi_lcd_print_menu(G_SAWTOOTH);
				 }else if(i == 3){
					 if(POINTER_G){
						 FREQ_G--;
						 period = MyRound((float)(1/TIME_OF_SAMPLE/FREQ_G));
						 devide = MyRound((float)(NUMBER_OF_SAMPLES*TIME_OF_SAMPLE*FREQ_G*COEFICIENT_MULTIPLE_DEC));
						 step = MyRound((float)255/period);
						 twi_lcd_print_freq(FREQ_G, POINTER_G);
					 }else{
						 VOLATGE_G--;
						 period = MyRound((float)(1/TIME_OF_SAMPLE/FREQ_G));
						 devide = MyRound((float)(NUMBER_OF_SAMPLES*TIME_OF_SAMPLE*FREQ_G*COEFICIENT_MULTIPLE_DEC));
						 step = MyRound((float)255/period);
						 twi_lcd_print_voltage(VOLATGE_G, POINTER_G);
					 }
				 }
			 }
			 if(value == 5){
				 uart_puts("COLLUM: 5\r\n");
				 if(i == 0){
					 STATE_G = G_SINE;
					 twi_lcd_print_menu(G_SINE);
				 }else if(i == 1){
					 STATE_G = G_TRIABGLE;
					 twi_lcd_print_menu(G_TRIABGLE);
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
						 FREQ_G++;
						 period = MyRound((float)(1/TIME_OF_SAMPLE/FREQ_G));
						 devide = MyRound((float)(NUMBER_OF_SAMPLES*TIME_OF_SAMPLE*FREQ_G*COEFICIENT_MULTIPLE_DEC));
						 twi_lcd_print_freq(FREQ_G, POINTER_G);
					 }else{
						 VOLATGE_G++;
						 period = MyRound((float)(1/TIME_OF_SAMPLE/FREQ_G));
						 devide = MyRound((float)(NUMBER_OF_SAMPLES*TIME_OF_SAMPLE*FREQ_G*COEFICIENT_MULTIPLE_DEC));
						 twi_lcd_print_voltage(VOLATGE_G, POINTER_G);
					 }
				 }
			 }
			 GPIO_write_low(&PORTD, PD4+i);
		 }
    }

    // Will never reach this
    return 0;
}
/*
ISR(TIMER1_COMPB_vect)
{
    GPIO_toggle(&PORTB, PB6);
}
*/
/* Interrupt service routines ----------------------------------------*/
/**
 * ISR starts when Timer/Counter0 overflows. Use single conversion mode
 * and start conversion four times per second.
 */

ISR(TIMER1_COMPA_vect, ISR_BLOCK){
//ISR(TIMER0_OVF_vect, ISR_BLOCK){
	static uint16_t c_time_pause = 0;
	static uint16_t c_time_freq = 0;
	static uint8_t is_null = 0;
	uint16_t value_long;
	button_t* get = NULL;
	c_time_pause++;
	GPIO_toggle(&PORTD, PD3);
	switch(STATE_G){
		case G_DTFM:
			if(c_time_pause > g_next_button->wave_lenght){
				c_time_pause = 0;
				DAC(ADC_ZERO_VALUE);
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
				DAC((uint8_t)(DAC_value >> 1));
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
			if(c_time_pause > g_next_button->wave_distance){
				c_time_pause = 0;
				STATE_G = G_FREE;
				break;
			}
			break;
		case G_FREE:
			get = (button_t*)FIFO_get();
			if(get != NULL){
				g_next_button = get;
				STATE_G = G_DTFM;
			}
			break;
	}


}

/*
ISR(TIMER0_OVF_vect, ISR_BLOCK){

}
*/
