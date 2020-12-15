/**
* @file include\FIFO.h
* @author Tomáš Vymazal
* @date 15 Dec 2020
* @copyright 2020 Tomáš Vymazal
* @brief Data structure of Buffer, implemented linked list with FIFO specification.
*/


#ifndef FIFO_H
#define FIFO_H

#include <avr/io.h>
#include <stdlib.h>

/*******************************************************************************
*
*								FUNCTIONS
*
*******************************************************************************/


/**
* @brief Init Buffer
* @details <details>
*/
void FIFO_init();

/**
* @brief add member into Buffer
* @param [in] <name> <parameter_description>
* @return 1 if add was successfully otherwise 0
*/
uint8_t FIFO_add(void *data);

/**
* @brief remove one member
* @return adrress into strucrure
* @details do not forget redefine return value into correct address
*/
void* FIFO_get();

#endif
