#include "adc.h"

#ifndef ADC_BASE_ADDR
#define ADC_BASE_ADDR 0x1000
#endif

void adc_init(void)
{
    xmem_init();
    DDRD |= (1 << PD4);
    TCCR3A = (1 << COM3A0);              // Toggle OC1A on Compare Match
    TCCR3B = (1 << WGM32) | (1 << CS30); // CTC mode, no prescaling
    OCR3A = 0;                           //(F_CPU / (2 * frequency)) - 1; // Set compare value for desired frequency
}

// Function to read from MAX156 ADC
uint8_t adc_read(uint8_t channel)
{
    volatile uint8_t *adc = (volatile uint8_t *)ADC_BASE_ADDR;
    adc[0] = channel | (1 << 7);
    _delay_us(5);
    uint8_t value[4] = {0, 0, 0, 0};
    for (int i = 0; i <= 3; i++)
    {
        value[i] = adc[0];
    }
    return value[channel];
}
