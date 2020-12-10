#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <avr/eeprom.h>
#include <stdlib.h>         // C library. Needed for conversion function
#include <math.h>
#include "uart.h"
#include "sinGeneratorEEPROM.h"

/*
void DAC(uint8_t value){
	PORT_L &= 0xF0;
	PORT_L |= value & 0xF;
	PORT_H &= 0xF0;
	PORT_H |= value >> 4;
}
*/

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



uint16_t MyRound1024(float value){
	if(value > 0){
		if(value < 1024){
			uint8_t v = (uint8_t)value;
			if((value - ((float)v)) < 0.5){
				return v;
			}
			return v+1;
		}
		return 1024;
	}
	return 0;
}




void generateSinIntoEEPROM(uint16_t numberOfpoints, const uint8_t* addr){
	//(uint8_t*)malloc(sizeof(uint8_t)*255);
	for(uint16_t i = 0; i < numberOfpoints ; ++i){
		//ret[i] = MyRound(127 + 128*sin(TWO_PI/(float)numberOfpoints*(uint8_t)i));
		eeprom_write_byte((uint8_t*)(addr+i), MyRound(127 + 128*sin(TWO_PI/(float)numberOfpoints*i)));
		//printf("%i -> to adrr: %p -> %i\n", i,(uint8_t*)(addr+(uint8_t)i), MyRound(128 + 127*sin(TWO_PI/(float)numberOfpoints*(uint8_t)i)));
	}
}
