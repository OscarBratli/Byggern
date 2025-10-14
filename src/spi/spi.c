#include "spi.h"


// USE SPI Mode 0
// Modes to decide clock polarity and phase
// Mode 0: CPOL = 0, CPHA = 0      <---- SPI MODE ==== SPI MODE 0




void SPI_MasterInit(void)
{
/* Set MOSI and SCK as outputs, preserve other DDR settings */
DDRB |= (1<<SPI_MOSI)|(1<<SPI_SCK);  // Use |= instead of = to preserve other pins!

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
/* Set MISO as output, preserve other DDR settings */
DDRB |= (1<<SPI_MISO);  // Use |= instead of = to preserve other pins!
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
    
    // Set MOSI, SCK, and CS pin as outputs for testing (if needed)
    DDRB |= (1 << SPI_MOSI) | (1 << SPI_SCK) | (1 << SPI_SS);  // MOSI, SCK, CS
    PORTB |= (1 << SPI_SS);  // CS high (deselected)

}
