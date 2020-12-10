#include <avr/io.h>
#include <util/delay.h>
#include "twi.h"
#include "uart.h"
#include "TWI_LCD_definitions.h"
#include "TWI_LCD.h"


void twi_lcd_print_number(uint16_t value){
	char lcd_string[5] = "00000";
	itoa(value, lcd_string, 10);
	twi_lcd_print(lcd_string);
	return;
}

void twi_lcd_printChar(char c){
	twi_lcd_send(c, TWI_LCD_DATA);
}

void twi_lcd_print(const char* data){
	uint8_t i = 0;
	while(data[i] != '\0'){
		twi_lcd_printChar(data[i]);
		i++;
	}
}

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

void twi_lcd_send(uint8_t data, uint8_t RS){

	uint8_t H_data = data >> 4;
	uint8_t L_data = (data & 0xF);

	twi_lcd_send_L4bit(H_data | RS);
	twi_lcd_send_L4bit(L_data | RS);

}

void twi_lcd_init(uint8_t star_parametrs){

	_delay_ms(TWI_LCD_START_DELAY);
	// Initialize I2C (TWI)
	twi_init();

	// call instruction for set into 4BIT mode
	twi_lcd_send_L4bit(TWI_LCD_SELECT_4BIT_MODE | TWI_LCD_INSTRUCTION);
	// call instruction for set ON display with params
	twi_lcd_send(TWI_LCD_DISPLAY | star_parametrs, TWI_LCD_INSTRUCTION);

}

void twi_lcd_set_lines(uint8_t line_4bit_define){
	twi_lcd_send(line_4bit_define, TWI_LCD_INSTRUCTION);
}

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

void twi_lcd_display_shift_left(){
	twi_lcd_send(TWI_LCD_MOVE | TWI_LCD_MOVE_DISP | TWI_LCD_MOVE_LEFT, TWI_LCD_INSTRUCTION);
}

void twi_lcd_display_shift_right(){
	twi_lcd_send(TWI_LCD_MOVE | TWI_LCD_MOVE_DISP | TWI_LCD_MOVE_RIGHT, TWI_LCD_INSTRUCTION);
}

void twi_lcd_clean(){
	twi_lcd_send(TWI_LCD_CLEAR_DISPLAY, TWI_LCD_INSTRUCTION);
}

void twi_lcd_data(uint8_t addr, uint8_t* data){
	twi_lcd_send(TWI_LCD_CGRAM | (addr & TWI_LCD_CGRAM_ADDRESS_MASK), TWI_LCD_INSTRUCTION);
	for(uint8_t i = 0 ; i < 8 ; ++i){
		twi_lcd_send(data[i], TWI_LCD_DATA);
	}
	twi_lcd_send(TWI_LCD_DDRAM, TWI_LCD_INSTRUCTION);
}
