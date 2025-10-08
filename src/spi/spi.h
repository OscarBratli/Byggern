#ifndef SPI_H
#define SPI_H

#include <avr/io.h>
#include <stdint.h>

// === SPI Pin definitions (ATmega162) ===
#define SPI_MOSI PB5
#define SPI_MISO PB6
#define SPI_SCK  PB7
#define SPI_SS   PB4

// === OLED control pins (handled by SPI layer) ===
#define OLED_DC   PD4   // Data/Command select
#define OLED_RES  PD5   // Reset

// === Public SPI functions ===
void spi_init(void);
void spi_select(void);
void spi_deselect(void);
uint8_t spi_transfer(uint8_t data);

// === OLED helpers for higher-level drivers ===
void oled_set_command_mode(void);
void oled_set_data_mode(void);
void oled_reset_pulse(void);

#endif
