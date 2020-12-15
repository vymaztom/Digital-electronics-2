/**
* @file include\FIFO.h
* @author Tomáš Vymazal
* @date 15 Dec 2020
* @copyright 2020 Tomáš Vymazal
* @brief Data structure of Buffer, implemented linked list with FIFO specification.
*/


#include <avr/io.h>
#include <stdlib.h>
#include "FIFO.h"



// structure of Buffer notes
typedef struct FIFO_note{
	void* data_addres;
	struct FIFO_note* next;
}FIFO_note_t;

// help variables defined start and end of linked list
FIFO_note_t* FIFO_first_note;
FIFO_note_t* FIFO_last_note;
uint16_t FIFO_size;

/*******************************************************************************
*
*								FUNCTIONS
*
*******************************************************************************/

// Init Buffer
void FIFO_init(){
	FIFO_size = 0;
	FIFO_first_note = NULL;
	FIFO_last_note = NULL;
}

// add member into Buffer
uint8_t FIFO_add(void *data){
	FIFO_note_t* one = (FIFO_note_t*)malloc(sizeof(FIFO_note_t));
	if(one == NULL){
		return 0;
	}
	one->data_addres = data;
	one->next = NULL;
	if(FIFO_size == 0){
		FIFO_first_note = one;
		FIFO_last_note = one;
		FIFO_size++;
	}else{
		FIFO_last_note->next = one;
		FIFO_last_note = one;
		FIFO_size++;
	}
	return 1;
}

// remove one member
void* FIFO_get(){
	if(FIFO_size > 0){
		void* ret = FIFO_first_note->data_addres;
		FIFO_note_t* one = FIFO_first_note;
		FIFO_first_note = FIFO_first_note->next;
		FIFO_size--;
		free(one);
		return ret;
	}
	return NULL;
}
