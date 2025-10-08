#include "adc.h"

#ifndef ADC_BASE_ADDR
#define ADC_BASE_ADDR 0x1000
#endif

void adc_init(void)
{
    xmem_init();
    
    // Clock generation on PB0 (pin 1) - Timer0 OC0
    // This is the standard approach from the working repository
    DDRB |= (1 << PB0);  // Set PB0 as output for clock
    
    // Configure Timer0 for CTC mode with toggle output (exactly like the repository)
    TCCR0 |= (1 << WGM01);               // CTC mode
    TCCR0 &= ~(1 << WGM00);              // Clear WGM00 for CTC
    
    // Toggle OC0 (PB0) on Compare Match
    TCCR0 &= ~(1 << COM01);
    TCCR0 |= (1 << COM00);
    
    // No prescaling - use internal clock directly
    TCCR0 &= ~(1 << CS02);
    TCCR0 &= ~(1 << CS01);
    TCCR0 |= (1 << CS00);
    
    // Set frequency (from repository: OCR0 = 0 gives 0.5 x F_CPU)
    OCR0 = 0;  // This generates the highest frequency (F_CPU/2 = 8MHz)
    
    // Wait for clock to stabilize
    _delay_ms(10);
}

// Function to read from MAX156 ADC
uint8_t adc_read(uint8_t channel)
{
    volatile uint8_t *adc = (volatile uint8_t *)ADC_BASE_ADDR;
    adc[0] = 0;
    _delay_us(100);
    uint8_t value[4] = {0, 0, 0, 0};
    for (int i = 0; i <= 3; i++)
    {
        value[i] = adc[0];
    }
    return value[channel];
}
