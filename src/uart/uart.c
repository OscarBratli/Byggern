#include "uart.h"

// RX globals 
volatile unsigned char uart_rx_data = 0;
volatile uint8_t uart_rx_flag = 0;

// Forward declarations
int uart_putchar(char c, FILE *stream);
int uart_getchar(FILE *stream);

void uart_init(unsigned int ubrr) {
    // Set baud rate
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;

    // Enable RX, TX and RX complete interrupt
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);   


    // Frame format: 8 data, 1 stop, no parity
    UCSR0C = (1 << URSEL0) | (1 << UCSZ01) | (1 << UCSZ00);

    // Connect stdio streams (printf + getchar/scanf)
    fdevopen(uart_putchar, uart_getchar);

    sei();
}

void uart_transmit(unsigned char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

int uart_putchar(char c, FILE *stream) {
   
    uart_transmit(c);
    return 0;
}

int uart_getchar(FILE *stream) {
    while (!(UCSR0A & (1 << RXC0)));  // wait until data received
    return UDR0;
}

/*
// RX interrupt (optional, not needed for getchar/scanf)
ISR(USART_RXC_vect) {
    uart_rx_data = UDR0;
    uart_rx_flag = 1;
}
    
*/
