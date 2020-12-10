#ifndef TWI_LCD_H
#define TWI_LCD_H

#include <avr/io.h>
#include <util/delay.h>
#include "twi.h"
#include "TWI_LCD_definitions.h"



#ifndef TWI_ADDRESS
	#define TWI_ADDRESS 32
#endif

#ifndef TWI_LCD_LINES
	#define TWI_LCD_LINES 2
#endif

#ifndef TWI_LCD_DISP_LENGTH
	#define TWI_LCD_DISP_LENGTH 16
#endif



#define TWI_LCD_START_DELAY 15

#define TWI_LCD_NULL_DATA 0b00000000
#define TWI_LCD_SELECT_4BIT_MODE 0b00000010
#define TWI_LCD_FUNCTION_4BIT_1LINE 0b00100000
#define TWI_LCD_FUNCTION_4BIT_2LINES 0b00101000


#define TWI_LCD_CLEAR_DISPLAY 0b00000001
#define TWI_LCD_CURSOR_HOME 0b00000010

#define TWI_LCD_MOVE 0b00010000
#define TWI_LCD_MOVE_DISP 0b00001000
#define TWI_LCD_MOVE_CURSOR 0b00000000
#define TWI_LCD_MOVE_RIGHT 0b00000100
#define TWI_LCD_MOVE_LEFT 0b00000000
#define TWI_LCD_CGRAM 0b01000000
#define TWI_LCD_CGRAM_ADDRESS_MASK 0b00111111
#define TWI_LCD_DDRAM 0b10000000



#define TWI_LCD_LINE_LENGTH 0x40
#define TWI_LCD_START_LINE1 0x00
#define TWI_LCD_START_LINE2 0x40
#define TWI_LCD_START_LINE3 0x14
#define TWI_LCD_START_LINE4 0x54


#define TWI_LCD_DISPLAY 0b00001000
#define TWI_LCD_DISPLAY_ON 0b00000100
#define TWI_LCD_DISPLAY_OFF 0b00000000
#define TWI_LCD_DISPLAY_CURSOR_ON 0b00000010
#define TWI_LCD_DISPLAY_CURSOR_OFF 0b00000000
#define TWI_LCD_DISPLAY_BLINK_ON 0b00000001
#define TWI_LCD_DISPLAY_BLINK_OFF 0x00000000


#define TWI_LCD_ENABLE_DATA_SET 0b00010000
#define TWI_LCD_ENABLE_DATA_SUBMIT 0b00000000

#define TWI_LCD_INSTRUCTION 0b00000000
#define TWI_LCD_DATA 0b00100000


void twi_lcd_send_L4bit(uint8_t data);
void twi_lcd_send(uint8_t data, uint8_t RS);
void twi_lcd_init(uint8_t star_parametrs);
void twi_lcd_printChar(char c);
void twi_lcd_print(const char* data);
void twi_lcd_set_lines(uint8_t line_4bit_define);
void twi_lcd_print_number(uint16_t value);
void twi_lcd_gotoxy(uint8_t x, uint8_t y);
void twi_lcd_display_shift_left();
void twi_lcd_display_shift_right();
void twi_lcd_clean();
void twi_lcd_data(uint8_t addr, uint8_t* data);
#endif
