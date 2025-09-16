#pragma once

#include <stdio.h>
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>


// === UART config ===
#define F_CPU 4915200UL
#define BAUD 9600
#define MYUBRR (F_CPU/16/BAUD - 1)

// === API ===
void uart_init(unsigned int ubrr);
void uart_transmit(unsigned char data);
void uart_send_string(const char *s);

// RX globals (set by ISR, read in main)
extern volatile unsigned char uart_rx_data;
extern volatile uint8_t uart_rx_flag;
