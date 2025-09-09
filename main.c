

#define F_CPU 4915200UL
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"



int main(void){

    DDRB |= (1<<PB0);

    while(1){

        PORTB ^= (1<<PB0);
        _delay_ms(500);
    }

}

