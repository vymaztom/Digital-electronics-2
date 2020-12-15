/**
* @file sinGeneratorEEPROM.c
* @author Tomáš Vymazal
* @date 15 Dec 2020
* @copyright 2020 Tomáš Vymazal
* @brief Support lib for project. Define function for round a flout number into uint8_t, function for generate sin data into EEPROM and structures.
*/



#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <avr/eeprom.h>
#include <stdlib.h>         // C library. Needed for conversion function
#include <math.h>
#include "uart.h"
#include "sinGeneratorEEPROM.h"

// Round flout into uint8_t
uint8_t MyRound(float value){
	if(value > 0){
		if(value < 255){
			uint8_t v = (uint8_t)value;
			if((value - ((float)v)) < 0.5){
				return v;
			}
			return v+1;
		}
		return 255;
	}
	return 0;
}

// Generate data into EEPROM
void generateSinIntoEEPROM(uint16_t numberOfpoints, const uint8_t* addr){
	for(uint16_t i = 0; i < numberOfpoints ; ++i){
		eeprom_write_byte((uint8_t*)(addr+i), MyRound(127 + 128*sin(TWO_PI/(float)numberOfpoints*i)));
	}
}
