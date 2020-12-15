#ifndef GPIO_H
#define GPIO_H

/***********************************************************************
 *
 * GPIO library for AVR-GCC.
 * ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
 *
 * Copyright (c) 2019-2020 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 *
 **********************************************************************/

/**
 * @file  gpio.h
 * @brief GPIO library for AVR-GCC.
 *
 * @details
 * The library contains functions for controlling AVRs' gpio pin(s).
 *
 * @note
 * Based on AVR Libc Reference Manual. Tested on ATmega328P (Arduino Uno),
 * 16 MHz, AVR 8-bit Toolchain 3.6.2.
 *
 * @copyright (c) 2019-2020 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 */

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>

/* Function prototypes -----------------------------------------------*/
/**
 * @brief Configure one output pin in Data Direction Register.
 * @param reg_name - Address of Data Direction Register, such as &DDRA,
 *                   &DDRB, ...
 * @param pin_num - Pin designation in the interval 0 to 7
 */
void GPIO_config_output(volatile uint8_t *reg_name, uint8_t pin_num);
/**
 * @brief Configure one input pin wihout pull-ap resistor in Data Direction Register.
 * @param reg_name - Address of Data Direction Register, such as &DDRA,
 *                   &DDRB, ...
 * @param pin_num - Pin designation in the interval 0 to 7
 */
void GPIO_config_input_nopull(volatile uint8_t *reg_name, uint8_t pin_num);
/**
 * @brief Configure one input pin with active pull-up resistor in Data Direction Register.
 * @param reg_name - Address of Data Direction Register, such as &DDRA,
 *                   &DDRB, ...
 * @param pin_num - Pin designation in the interval 0 to 7
 */
void GPIO_config_input_pullup(volatile uint8_t *reg_name, uint8_t pin_num);
/**
 * @brief Configure one output pin to low stata in Data Register.
 * @param reg_name - Address of Data Register, such as &PORTA,
 *                   &PORTB, ...
 * @param pin_num - Pin designation in the interval 0 to 7
 */
void GPIO_write_low(volatile uint8_t *reg_name, uint8_t pin_num);
/**
 * @brief Configure one output pin to hight stata in Data Register.
 * @param reg_name - Address of Data Register, such as &PORTA,
 *                   &PORTB, ...
 * @param pin_num - Pin designation in the interval 0 to 7
 */
void GPIO_write_high(volatile uint8_t *reg_name, uint8_t pin_num);
/**
 * @brief Configure one output pin to opposed value in Data Register.
 * @param reg_name - Address of Data Register, such as &PORTA,
 *                   &PORTB, ...
 * @param pin_num - Pin designation in the interval 0 to 7
 */
void GPIO_toggle(volatile uint8_t *reg_name, uint8_t pin_num);
/**
 * @brief Return value of one input pin in Input Pins Address.
 * @param reg_name - Address of Input Pins Address, such as &PINA,
 *                   &PINB, ...
 * @param pin_num - Pin designation in the interval 0 to 7
 */
uint8_t GPIO_read(volatile uint8_t *reg_name, uint8_t pin_num);
/**
 * @brief Return value (uint16_t) AD converotr.
 * @param pin_num - Pin designation in the interval 0 to 7
 */
uint16_t GPIO_analogRead10(uint8_t pin_num);

#endif
