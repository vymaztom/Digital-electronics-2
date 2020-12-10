#include <avr/io.h>
#include <stdlib.h>
#include "FIFO.h"

typedef struct FIFO_note{
	void* data_addres;
	struct FIFO_note* next;
}FIFO_note_t;

FIFO_note_t* FIFO_first_note;
FIFO_note_t* FIFO_last_note;
uint16_t FIFO_size;

void FIFO_init(){
	FIFO_size = 0;
	FIFO_first_note = NULL;
	FIFO_last_note = NULL;
}

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
