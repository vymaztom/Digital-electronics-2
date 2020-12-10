#ifndef SINGENERATOREEPROM_H
#define SINGENERATOREEPROM_H

#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include <avr/eeprom.h>
#include <stdlib.h>         // C library. Needed for conversion function
#include <math.h>

#ifndef TWO_PI
	#define TWO_PI 6.283185307179586476925286766559
#endif

#ifndef DEG_TO_RAD
	#define DEG_TO_RAD 0.017453292519943295769236907684886
#endif

#ifndef RAD_TO_DEG
	#define RAD_TO_DEG 57.295779513082320876798154814105
#endif



#define PORT_L_MASK 0xf
#define PORT_H_OFFSET 4
#define PORT_L PORTC
#define PORT_H PORTB
#define DAC_L(value) PORT_L &= 0xF0; PORT_L |= (value) & PORT_L_MASK
#define DAC_H(value) PORT_H &= 0xF0; PORT_H |= (value) >> PORT_H_OFFSET
//#define DAC(value) DAC_H((value)); DAC_L((value))
#define DAC(value) PORTB = value;

//void DAC(uint8_t value);

uint8_t MyRound(float value);

uint16_t MyRound1024(float value);

void generateSinIntoEEPROM(uint16_t numberOfpoints, const uint8_t* addr);

#endif
