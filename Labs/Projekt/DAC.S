;* Includes ------------------------------------------------------------
; Set offset for control register addresses (NEEDED FOR I/O REGISTERS)
#define __SFR_OFFSET    0
#include <avr/io.h>
#include <stdlib.h>

;* Define --------------------------------------------------------------
#define result  r16
#define temp    r17
#define input   r24

;* Functions -----------------------------------------------------------
;* Set register PORTB into value.
;* Input:  r24 - Current value of shift register
.global DAC
DAC:
	out PORTB, input			; Write input into PORTB register
	ret

;*----------------------------------------------------------------------
