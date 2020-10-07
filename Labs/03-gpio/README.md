# Lab 3: User library for GPIO control


## Preparation tasks (done before the lab at home)

| **Data type** | **Number of bits** | **Range** | **Description** |
| :-: | :-: | :-: | :-- |
| `uint8_t`  | 8 | 0, 1, ..., 255 | Unsigned 8-bit integer |
| `int8_t`   | 8 | -128 – 127 | Signed 8-bit integer |
| `uint16_t` | 16 | 0 - 65535 | Unsigned 16-bit integer |
| `int16_t`  | 16 | -32768 - 32767 | Signed 16-bit integer |
| `float`    | 32 | -3.4e+38, ..., 3.4e+38 | Single-precision floating-point |
| `void`     | 8 | - | none value |

```C
#include <avr/io.h>

// Function declaration (prototype)
uint16_t calculate(uint8_t x, uint8_t y);

int main(void)
{
    uint8_t a = 156;
    uint8_t b = 14;
    uint16_t c;

    // Function call
    c = calculate(a, b);

    while (1)
    {
    }
    return 0;
}

// Function definition (body)
uint16_t calculate(uint8_t x, uint8_t y)
{
    uint16_t result;    // result = x^2 + 2xy + y^2

    result = x*x;
	result += 2*x*y;
	result += y*y;
    return result;
}
```





### GPIO Lib HELP

| **Return** | **Function name** | **Function parameters** | **Description** |
| :-: | :-- | :-- | :-- |
| `void` | `GPIO_config_output` | `volatile uint8_t *reg_name, uint8_t pin_num` | Configure one output pin in Data Direction Register |
| `void` | `GPIO_config_input_nopull` | `volatile uint8_t *reg_name, uint8_t pin_num` | Configure one input pin in DDR without pull-up resistor |
| `void` | `GPIO_config_input_pullup` | `volatile uint8_t *reg_name, uint8_t pin_num` | Configure one input pin in DDR and enable pull-up resistor |
| `void` | `GPIO_write_low` | `volatile uint8_t *reg_name, uint8_t pin_num` | Set one output pin in PORT register to low |
| `void` | `GPIO_write_high` | `volatile uint8_t *reg_name, uint8_t pin_num` | Set one output pin in PORT register to high |
| `void` | `GPIO_toggle` | `volatile uint8_t *reg_name, uint8_t pin_num` | Toggle one output pin value in PORT register |
| `uint8_t` | `GPIO_read` | `volatile uint8_t *reg_name, uint8_t pin_num` | Get input pin value from PIN register |


## Final application

```C

/***********************************************************************
*
* Alternately toggle two LEDs when a push button is pressed. Use
* functions from GPIO library.
* ATmega328P (Arduino Uno), 16 MHz, AVR 8-bit Toolchain 3.6.2
*
* Copyright (c) 2019-2020 Tomas Fryza
* Dept. of Radio Electronics, Brno University of Technology, Czechia
* This work is licensed under the terms of the MIT license.
*
**********************************************************************/

/* Defines -----------------------------------------------------------*/
#define LED_GREEN   PB5		// green LED
#define LED_RED     PC0		// red	 LED
#define BUTTON     	PD0		// Button
#define BLINK_DELAY 500

#ifndef F_CPU
#define F_CPU 16000000      // CPU frequency in Hz required for delay
#endif

/* Includes ----------------------------------------------------------*/
#include <util/delay.h>     // Functions for busy-wait delay loops
#include <avr/io.h>         // AVR device-specific IO definitions
#include "gpio.h"           // GPIO library for AVR-GCC


/* Function definitions ----------------------------------------------*/
/**
* Main function where the program execution begins. Toggle two LEDs
* when a push button is pressed. Functions from user-defined GPIO
* library is used instead of low-level logic operations.
*/
int main(void)
{

	/* GREEN LED */
	GPIO_config_output(&DDRB, LED_GREEN);
	GPIO_write_low(&PORTB, LED_GREEN);

	/* RED LED */
	GPIO_config_output(&DDRC, LED_RED);
	GPIO_write_high(&PORTC, LED_RED);

	/* BUTTON */
	GPIO_config_input_pullup(&DDRD, BUTTON);

	while (1){

		_delay_ms(BLINK_DELAY);

		if(GPIO_read(&PIND, BUTTON) == 0)
		{
			GPIO_toggle(&PORTB, LED_GREEN);
			GPIO_toggle(&PORTC, LED_RED);
		}

	}

	return 0;
}



```

## Demonstration

![03-leds](../../Images/03-leds.gif)


## Lab assignment

1. Preparation tasks (done before the lab at home). Submit:
    * Table with data types,
    * Completed source code from the example.

2. GPIO library. Submit:
    * Listing of library source file `gpio.c`,
    * C code of the application `main.c`,
    * In your words, describe the difference between the declaration and the definition of the function in C. Give an example.
