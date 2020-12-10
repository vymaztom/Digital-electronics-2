#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>


#define TWO_PI 6.283185307179586476925286766559

uint8_t MyRound(float value){
	if(value > 0){
		if(value < 255){
			uint8_t v = (uint8_t)value;
			if((value - ((float)v)) < 0.5){
				return v;
			}
			return v+1;
		}
		return 255;
	}
	return 0;
}

void generateSinIntoEEPROM(uint8_t numberOfpoints, const uint8_t* addr){
	for(uint16_t i = 0; i <= numberOfpoints ; ++i){
		eeprom_write_byte((uint8_t*)(addr+i), MyRound(127 + 127*sin(TWO_PI/(float)numberOfpoints*i)));
		printf("%i -> to adrr: %p -> %i\n", i,(uint8_t*)(addr+(uint8_t)i), MyRound(128 + 127*sin(TWO_PI/(float)numberOfpoints*(uint8_t)i)));
	}
}



int main(){
	generateSinIntoEEPROM(255, NULL);
	return 0;
}
