#include "spi.h"
#include <util/delay.h>
#include "uart/uart.h"
#include "cpu_time/cpu_time.h"


// USE SPI Mode 0
// Modes to decide clock polarity and phase
// Mode 0: CPOL = 0, CPHA = 0      <---- SPI MODE ==== SPI MODE 0




void SPI_MasterInit(void)
{
/* Set MOSI and SCK output, all others input */
DDRB = (1<<SPI_MOSI)|(1<<SPI_SCK);

/* Enable SPI, Master, set clock rate fck/16, Mode 0 (CPOL=0, CPHA=0) */
SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);  // CPOL=0, CPHA=0 (bits default to 0)


// Explicitly ensure CPOL=0 and CPHA=0 for SPI Mode 0
SPCR &= ~(1<<CPOL);  // Clear CPOL bit (Clock Polarity = 0)
SPCR &= ~(1<<CPHA);  // Clear CPHA bit (Clock Phase = 0)
}


void SPI_MasterTransmit(char cData)
{
/* Start transmission */
SPDR = cData;
/* Wait for transmission complete */
while(!(SPSR & (1<<SPIF)))
;
}

void SPI_SlaveInit(void)
{
/* Set MISO output, all others input */
DDRB = (1<<SPI_MISO);
/* Enable SPI */
SPCR = (1<<SPE);
}

char SPI_SlaveReceive(void)
{
/* Wait for reception complete */
while(!(SPSR & (1<<SPIF)))
;
/* Return data register */
return SPDR;
}

void SPI_Select(void)
{
/* Pull CS low to select slave */
PORTB &= ~(1<<SPI_SS);
}

void SPI_Deselect(void)
{
/* Pull CS high to deselect slave */
PORTB |= (1<<SPI_SS);
}

uint8_t SPI_Transfer(uint8_t data)
{
/* Start transmission */
SPDR = data;
/* Wait for transmission complete */
while(!(SPSR & (1<<SPIF)))
;
/* Return received data */
return SPDR;
}





// SPI Setup Function (call once during initialization)
void spi_setup(void) 
{
    uart_init(MYUBRR);
    printf("SPI Test Starting...\r\n");
    
    // Initialize SPI
    SPI_MasterInit();
    
    // Set MOSI, SCK, and a CS pin as outputs for testing
    DDRB |= (1 << SPI_MOSI) | (1 << SPI_SCK) | (1 << SPI_SS);  // MOSI, SCK, CS
    PORTB |= (1 << SPI_SS);  // CS high (deselected)

}

// SPI Loop Function (call repeatedly in main loop)
void spi_loop(void)
{
    
    // Select device using proper SPI function
    SPI_Select();
    _delay_us(10);
    
    // Send test pattern: 0x55 (01010101) - easy to see on scope
    printf("Sending 0x55...\r\n");
    SPI_MasterTransmit(0x55);
    _delay_ms(900);
    
    // Send test pattern: 0xAA (10101010) - opposite pattern  
    SPI_MasterTransmit(0xAA);
    _delay_ms(900);

    // Send test pattern: 0xFF (11111111) - all high
    SPI_MasterTransmit(0xFF);
    _delay_ms(900);

    // Send test pattern: 0x00 (00000000) - all low
    SPI_MasterTransmit(0x00);
    _delay_ms(900);
    
    // Deselect device using proper SPI function
    SPI_Deselect();
    _delay_ms(500);
} 