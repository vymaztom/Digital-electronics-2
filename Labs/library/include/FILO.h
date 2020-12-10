#ifndef FILO_H
#define FILO_H

#include <avr/io.h>
#include <stdlib.h>



void FILO_init();

uint8_t FILO_add(void *data);

void* FILO_get();

#endif
