

#include "uart.h"
#include <avr/io.h>

void uart0_init(uint32_t baud)
{
    // Calculate UBRR for normal speed (U2X0=0)
    uint16_t ubrr = (F_CPU / (16UL * baud)) - 1;

    // Set baud rate
    UBRRH = (uint8_t)(ubrr >> 8);   // URSEL=0 here because we write UBRRH
    UBRRL = (uint8_t)(ubrr & 0xFF); // Writing UBRRL triggers prescaler update on this AVR

    // Frame format: Async, No parity, 1 stop, 8 data:
    // URSEL=1 to write UCSRC on ATmega162; UMSEL=0; UPM1:0=00; USBS=0; UCSZ1:0=11
    UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);

    // Enable transmitter (TXEN0). (We can enable RX later when needed.)
    UCSRB = (1 << TXEN) | (0 << RXEN) | (0 << UCSZ2);
}

void uart0_putc(char c)
{
    // Wait for transmit buffer empty (UDRE set)
    while (!(UCSRA & (1 << UDRE))) { /* spin */ }
    UDR = c;
}

void uart0_puts(const char *s)
{
    while (*s) {
        uart0_putc(*s++);
    }
}
