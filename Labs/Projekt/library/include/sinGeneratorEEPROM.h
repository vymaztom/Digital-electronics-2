/**
* @file sinGeneratorEEPROM.c
* @author Tomáš Vymazal
* @date 15 Dec 2020
* @copyright 2020 Tomáš Vymazal
* @brief Support lib for project. Define function for round a flout number into uint8_t, function for generate sin data into EEPROM and structures.
*/

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




/**
* @brief Structure for define frequency and data for generating wave
*/
typedef struct freq{
	uint8_t c_time;
	uint8_t c_period;
	uint8_t c_devider;
	uint16_t frequency;
	uint8_t* data_address;
}freq_t;

/**
* @brief generator states
*/
enum STATE_GENERATOR{
	G_DTFM,
	G_SINE,
	G_SQUARE,
	G_TRIABGLE,
	G_SAWTOOTH,
	G_PAUSE,
	G_FREE
};

/**
* @brief Pressed button, definned by to frequency
*/
typedef struct button{
	freq_t* freq_1;
	freq_t* freq_2;
}button_t;


#define DAC_FAST(value) PORTB = value;  /**< write data into DAC faster than assembler function DAC() */

/**
* @brief Round flout into uint8_t
* @param float value input float value
* @return uint8_t value from 0 to 255
*/
uint8_t MyRound(float value);


/**
* @brief Generate data into EEPROM
* @param uint16_t numberOfpoints define number of samples for general sine wave
* @param const uint8_t* addr start address of data
*/
void generateSinIntoEEPROM(uint16_t numberOfpoints, const uint8_t* addr);

#endif
