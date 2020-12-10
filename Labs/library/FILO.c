#include <avr/io.h>
#include <stdlib.h>
#include "FILO.h"

typedef struct FILO_note{
	void* data_addres;
	struct FILO_note* next;
}FILO_note_t;

FILO_note_t* FILO_first_note;
FILO_note_t* FILO_last_note;
uint16_t FILO_size;

void FILO_init(){
	FILO_size = 0;
	FILO_first_note = NULL;
	FILO_last_note = NULL;
}

uint8_t FILO_add(void *data){
	FILO_note_t* one = (FILO_note_t*)malloc(sizeof(FILO_note_t));
	if(one == NULL){
		return 0;
	}
	one->data_addres = data;
	one->next = NULL;
	if(FILO_size == 0){
		FILO_first_note = one;
		FILO_last_note = one;
		FILO_size++;
	}else{
		FILO_last_note->next = one;
		FILO_last_note = one;
		FILO_size++;
	}
	return 1;
}

void* FILO_get(){
	if(FILO_size > 0){
		void* ret = FILO_first_note->data_addres;
		FILO_note_t* one = FILO_first_note;
		FILO_first_note = FILO_first_note->next;
		FILO_size--;
		free(one);
		return ret;
	}
	return NULL;
}
