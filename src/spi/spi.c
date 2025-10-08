#include "spi.h"
#include <util/delay.h>

void spi_init(void)
{
    // SPI lines
    DDRB |= (1 << SPI_MOSI) | (1 << SPI_SCK) | (1 << SPI_SS);
    DDRB &= ~(1 << SPI_MISO);
    PORTB |= (1 << SPI_SS); // deselect (active low)

    // OLED control lines
    DDRD |= (1 << OLED_DC) | (1 << OLED_RES);
    PORTD |= (1 << OLED_RES); // keep high (inactive)
    PORTD |= (1 << OLED_DC);  // default to data mode

    // Enable SPI: master mode, fosc/16, mode 0
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void spi_select(void)
{
    PORTB &= ~(1 << SPI_SS);
}

void spi_deselect(void)
{
    PORTB |= (1 << SPI_SS);
}

uint8_t spi_transfer(uint8_t data)
{
    SPDR = data;
    while (!(SPSR & (1 << SPIF)));
    return SPDR;
}

// === OLED helper control lines ===
void oled_set_command_mode(void)
{
    PORTD &= ~(1 << OLED_DC);
}

void oled_set_data_mode(void)
{
    PORTD |= (1 << OLED_DC);
}

void oled_reset_pulse(void)
{
    PORTD &= ~(1 << OLED_RES);
    _delay_ms(10);
    PORTD |= (1 << OLED_RES);
    _delay_ms(10);
}
