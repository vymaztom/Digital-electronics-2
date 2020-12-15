#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "twi.h"
#include "uart.h"
#include "TWI_LCD_definitions.h"
#include "TWI_LCD.h"


/*******************************************************************************
*
*						BLOCK OF FUNCTIONS FOR GENERATOR
*
*******************************************************************************/

// print to act menu new value of voltage variable
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

// print to act menu new value of frequency variable
void twi_lcd_print_freq(uint16_t value, uint8_t pointer){
	twi_lcd_gotoxy(5, 1);
	if(pointer == 1){
		twi_lcd_print("*");
	}else{
		twi_lcd_print(" ");
	}
	twi_lcd_print("f= ");
	twi_lcd_print_number(value);
	twi_lcd_print(" ");
	twi_lcd_gotoxy(13, 1);
	twi_lcd_print(" Hz");
}

// print to LCD actual menu by state with seted params
void twi_lcd_print_menu(enum STATE_GENERATOR kind, uint16_t VOLATGE_G, uint16_t FREQ_G, uint8_t POINTER_G){
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

		default:
		twi_lcd_print("PAUSE");
		break;
	}
}

/*******************************************************************************
*
*							BLOCK OF FUNCTIONS
*
*******************************************************************************/

// Function for print number
void twi_lcd_print_number(uint16_t value){
	char lcd_string[5] = "00000";
	itoa(value, lcd_string, 10);
	twi_lcd_print(lcd_string);
	return;
}

// Print one char into LCD
void twi_lcd_printChar(char c){
	twi_lcd_send(c, TWI_LCD_DATA);
}

// Print string int LCD
void twi_lcd_print(const char* data){
	uint8_t i = 0;
	while(data[i] != '\0'){
		twi_lcd_printChar(data[i]);
		i++;
	}
}

// Send 4 bits into TWI controler
void twi_lcd_send_L4bit(uint8_t data){
	// twi start communication
	twi_start((TWI_ADDRESS<<1) + TWI_WRITE);
	// set data into i2c controller
	twi_write(TWI_LCD_ENABLE_DATA_SET | data);
	_delay_us(750);
	// submit data into i2c controller
	twi_write(TWI_LCD_ENABLE_DATA_SUBMIT | data);
	twi_stop();
	_delay_us(750);
}

// Send 8 bits into TWI controler
void twi_lcd_send(uint8_t data, uint8_t RS){

	uint8_t H_data = data >> 4;
	uint8_t L_data = (data & 0xF);

	twi_lcd_send_L4bit(H_data | RS);
	twi_lcd_send_L4bit(L_data | RS);

}

// Init LCD, and TWI
void twi_lcd_init(uint8_t start_parametrs){

	_delay_ms(TWI_LCD_START_DELAY);
	// Initialize I2C (TWI)
	twi_init();

	// call instruction for set into 4BIT mode
	twi_lcd_send_L4bit(TWI_LCD_SELECT_4BIT_MODE | TWI_LCD_INSTRUCTION);
	// call instruction for set ON display with params
	twi_lcd_send(TWI_LCD_DISPLAY | start_parametrs, TWI_LCD_INSTRUCTION);
	// set 4 bit mode with 2 lines
	twi_lcd_set_lines();
}

// Set dispaly into 4 bit mode with 2 lines
void twi_lcd_set_lines(){
	twi_lcd_send(TWI_LCD_FUNCTION_4BIT_2LINES, TWI_LCD_INSTRUCTION);
}

// Move cursor into position
void twi_lcd_gotoxy(uint8_t x, uint8_t y){
	#if TWI_LCD_LINES == 1
	twi_lcd_send(TWI_LCD_DDRAM | TWI_LCD_START_LINE1 | x, TWI_LCD_INSTRUCTION);
	#endif

	#if TWI_LCD_LINES == 2
	if(y == 0){
		twi_lcd_send(TWI_LCD_DDRAM | TWI_LCD_START_LINE1 | x, TWI_LCD_INSTRUCTION);
	}else{
		twi_lcd_send(TWI_LCD_DDRAM | TWI_LCD_START_LINE2 | x, TWI_LCD_INSTRUCTION);
	}
	#endif

	#if TWI_LCD_LINES == 4
	if(y == 0){
		twi_lcd_send(TWI_LCD_DDRAM | TWI_LCD_START_LINE1 | x, TWI_LCD_INSTRUCTION);
	}else if(y == 1){
		twi_lcd_send(TWI_LCD_DDRAM | TWI_LCD_START_LINE2 | x, TWI_LCD_INSTRUCTION);
	}else if(y == 2){
		twi_lcd_send(TWI_LCD_DDRAM | TWI_LCD_START_LINE3 | x, TWI_LCD_INSTRUCTION);
	}else{
		twi_lcd_send(TWI_LCD_DDRAM | TWI_LCD_START_LINE3 | x, TWI_LCD_INSTRUCTION);
	}
	#endif
}

// Move text on left
void twi_lcd_display_shift_left(){
	twi_lcd_send(TWI_LCD_MOVE | TWI_LCD_MOVE_DISP | TWI_LCD_MOVE_LEFT, TWI_LCD_INSTRUCTION);
}

// Move text on right
void twi_lcd_display_shift_right(){
	twi_lcd_send(TWI_LCD_MOVE | TWI_LCD_MOVE_DISP | TWI_LCD_MOVE_RIGHT, TWI_LCD_INSTRUCTION);
}

// Clean display
void twi_lcd_clean(){
	twi_lcd_send(TWI_LCD_CLEAR_DISPLAY, TWI_LCD_INSTRUCTION);
}

// Write data into internal CGROM
void twi_lcd_data(uint8_t addr, uint8_t* data){
	twi_lcd_send(TWI_LCD_CGRAM | (addr & TWI_LCD_CGRAM_ADDRESS_MASK), TWI_LCD_INSTRUCTION);
	for(uint8_t i = 0 ; i < 8 ; ++i){
		twi_lcd_send(data[i], TWI_LCD_DATA);
	}
	twi_lcd_send(TWI_LCD_DDRAM, TWI_LCD_INSTRUCTION);
}
