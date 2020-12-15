/**
* @file include\TWI_LCD.h
* @author Tomáš Vymazal
* @date 15 Dec 2020
* @copyright 2020 Tomáš Vymazal
* @brief Library for control LCD by using TWI
*/

#ifndef TWI_LCD_H
#define TWI_LCD_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "twi.h"
#include "TWI_LCD_definitions.h"
#include "sinGeneratorEEPROM.h"




#ifndef TWI_ADDRESS
	#define TWI_ADDRESS 32								/**< Adrress of TWI controler */
#endif

#ifndef TWI_LCD_LINES
	#define TWI_LCD_LINES 2								/**< number of lines of LCD */
#endif

#ifndef TWI_LCD_DISP_LENGTH
	#define TWI_LCD_DISP_LENGTH 16						/**< display lenght of LCD */
#endif



#define TWI_LCD_START_DELAY 15							/**< Delay in ms on start function init */



/*******************************************************************************
*
*						DEFINE VARIABLES TO INIT DISPLAY
*
*******************************************************************************/

#define TWI_LCD_NULL_DATA 0b00000000
#define TWI_LCD_SELECT_4BIT_MODE 0b00000010
#define TWI_LCD_FUNCTION_4BIT_1LINE 0b00100000
#define TWI_LCD_FUNCTION_4BIT_2LINES 0b00101000


/*******************************************************************************
*
*						DEFINE SPERIAL INSTRUCTIONS
*
*******************************************************************************/

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

/*******************************************************************************
*
*					DEFINE MAPING FOR LINES OF DISPLAY
*
*******************************************************************************/

#define TWI_LCD_LINE_LENGTH 0x40
#define TWI_LCD_START_LINE1 0x00
#define TWI_LCD_START_LINE2 0x40
#define TWI_LCD_START_LINE3 0x14
#define TWI_LCD_START_LINE4 0x54


/*******************************************************************************
*
*							DEFINE INTI CONDITIONS
*
*******************************************************************************/

#define TWI_LCD_DISPLAY 0b00001000
#define TWI_LCD_DISPLAY_ON 0b00000100
#define TWI_LCD_DISPLAY_OFF 0b00000000
#define TWI_LCD_DISPLAY_CURSOR_ON 0b00000010
#define TWI_LCD_DISPLAY_CURSOR_OFF 0b00000000
#define TWI_LCD_DISPLAY_BLINK_ON 0b00000001
#define TWI_LCD_DISPLAY_BLINK_OFF 0x00000000

/*******************************************************************************
*
*				DEFINE VARABLES FOR CONTORL COMUNICATION
*
*******************************************************************************/

#define TWI_LCD_ENABLE_DATA_SET 0b00010000
#define TWI_LCD_ENABLE_DATA_SUBMIT 0b00000000

#define TWI_LCD_INSTRUCTION 0b00000000
#define TWI_LCD_DATA 0b00100000


/*******************************************************************************
*
*							BLOCK OF FUNCTIONS
*
*******************************************************************************/



/**
* @brief Send 4 bits into TWI controler
* @param uint8_t data send 4 LAST bit into TWI
*/
void twi_lcd_send_L4bit(uint8_t data);
/**
* @brief Send 8 bits into TWI controler
* @param uint8_t data send 8 bit into TWI
* @param uint8_t RS send RS bit value into TWI
*/
void twi_lcd_send(uint8_t data, uint8_t RS);
/**
* @brief Init LCD, and TWI
* @param uint8_t star_parametrs mask of start condition
* @details start_parametrs = TWI_LCD_DISPLAY_ON | TWI_LCD_DISPLAY_BLINK_ON ...
*/
void twi_lcd_init(uint8_t start_parametrs);
/**
* @brief Print one char into LCD
* @param char c value to print
* @details value of char must be from 33 to 127, for regular europe chars
*/
void twi_lcd_printChar(char c);
/**
* @brief Print string int LCD
* @param const char* data String value to print
* @details intput must be string or list of chars witch ends with '\0'
*/
void twi_lcd_print(const char* data);
/**
* @brief Set dispaly into 4 bit mode with 2 lines
* @details Called in intit function
*/
void twi_lcd_set_lines();
/**
* @brief Function for print number
* @param uint16_t value value to print
* @details unsigned discreet value
*/
void twi_lcd_print_number(uint16_t value);
/**
* @brief Move cursor into position
* @param uint8_t x vertical value
* @param uint8_t y horizontal value
*/
void twi_lcd_gotoxy(uint8_t x, uint8_t y);
/**
* @brief Move text on left
* @details move all chars into left
*/
void twi_lcd_display_shift_left();
/**
* @brief Move text on right
* @details move all chars into right
*/
void twi_lcd_display_shift_right();
/**
* @brief Clean display
* @details Clean all LCD and go to home
*/
void twi_lcd_clean();
/**
* @brief Write data into internal CGROM
* @param uint8_t addr address into CGROM
* @param uint8_t* data matrix of data lenght of 8
*/
void twi_lcd_data(uint8_t addr, uint8_t* data);

/*******************************************************************************
*
*						BLOCK OF FUNCTIONS FOR GENERATOR
*
*******************************************************************************/

/**
* @brief print to act menu new value of voltage variable
* @param uint16_t value global value of setted max voltage
* @param uint8_t pointer global value 0, 1, wiht define selected variable in menu
* @details called only if global voltage had been changed
*/
void twi_lcd_print_voltage(uint8_t value, uint8_t pointer);
/**
* @brief print to act menu new value of frequency variable
* @param uint16_t value global value of setted frequency
* @param uint8_t pointer global value 0, 1, wiht define selected variable in menu
* @details called only if global frequency had been changed
*/
void twi_lcd_print_freq(uint16_t value, uint8_t pointer);
/**
* @brief print to LCD actual menu by state with seted params
* @param enum STATE_GENERATOR  kind actual state
* @param uint16_t VOLATGE_G global value of setted max voltage
* @param uint16_t FREQ_G global value of setted frequency
* @param uint8_t POINTER_G global value 0, 1, wiht define selected variable in menu
* @details called only if state had been changed
*/
void twi_lcd_print_menu(enum STATE_GENERATOR kind, uint16_t VOLATGE_G, uint16_t FREQ_G, uint8_t POINTER_G);


#endif
