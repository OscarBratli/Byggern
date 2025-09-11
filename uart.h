/**
 * This module will contain UART communication tools.
 */

#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include "utils.h"

#ifndef UART_H
#define UART_H

#ifndef F_CPU
#  define F_CPU 16000000UL   // change if your clock differs
#endif

void uart0_init(uint32_t baud);
/** Blocks until TX buffer is free, then sends one char */
void uart0_putc(char c);
/** Convenience: send a C-string (no newline added) */
void uart0_puts(const char *s);

#endif
#define UBRR FOSC/16/UART_BAUD-1

void uart_init(unsigned int ubrr);
int uart_put_char(char data, FILE * file);
int uart_get_char(FILE * file);