#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


uint8_t calc_parity_8(int8_t value){
   value ^= (value >> 1);
   value ^= (value >> 2);
   value ^= (value >> 4);
   return(value & 1);
}

int main() {
   char c = 0x44;
   scanf("%c",&c);
   uint8_t ret = calc_parity_8((int8_t)c);
   printf("%i >>> ret is %i\n", c, ret);
   return 0;
}
