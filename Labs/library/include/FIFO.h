#ifndef FIFO_H
#define FIFO_H

#include <avr/io.h>
#include <stdlib.h>



void FIFO_init();

uint8_t FIFO_add(void *data);

void* FIFO_get();

#endif
