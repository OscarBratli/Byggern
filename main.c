
#define F_CPU 4915200UL
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"



int main(void)
{
    uart0_init(9600);

    // One-time sanity byte to the PC terminal
    uart0_putc('A');        // scope/terminal should see 0x41
    uart0_puts(" Hello, RS-232!\r\n");

    // Optional: blink forever or keep sending
    for (;;) {
        _delay_ms(1000);
        uart0_putc('.');    // heartbeats every second
    }
}

