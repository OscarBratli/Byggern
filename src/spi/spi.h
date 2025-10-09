#ifndef SPI_H
#define SPI_H

#include <avr/io.h>
#include <stdint.h>

// === SPI Pin definitions (ATmega162) - Matching your actual wiring ===
#define SPI_MOSI PB5    // Pin 3 MOSI -> PB5 ✓
#define SPI_MISO PB6    // Pin 4 MISO -> PB6 ✓  
#define SPI_SCK  PB7    // Pin 5 SCK -> PB7 ✓
#define SPI_SS   PB4    // Pin 2 IO_CS -> PB4 ✓

// === OLED control pins - Matching your actual wiring ===
#define OLED_DC   PB2   // Pin 0 (DISP_D/C) -> PB2 ✓
#define OLED_CS   PB3   // Pin 1 (DISP_CS) -> PB3 ✓
#define OLED_RES  PD5   // Pin 6 (DISP_RES) -> PD5 ✓

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
