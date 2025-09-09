/**
 * This module will contain UART communication tools.
 */
#pragma once

#include <avr/io.h>
#include <stdio.h>

#include "utils.h"

#define UART_BAUD 9600
#define UBRR FOSC/16/UART_BAUD-1

void uart_init(unsigned int ubrr);
int uart_put_char(char data, FILE * file);
int uart_get_char(FILE * file);