#include "adc.h"

#include "avr/io.h"
#include <util/delay.h>

volatile uint8_t* ADC_BASE_ADDR = 0x1000; 

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

// Alternative method for hard-wired mode - longer delays
ADC_values_t adc_read()
{

    volatile uint8_t *adc_base = (volatile uint8_t *)ADC_BASE_ADDR;
    
    *adc_base = 0;  // Dummy write to initiate read
    _delay_us(100); // Wait for the conversion to finish
    
   ADC_values_t data;
   //data.joystick_x = *adc_base;
   //data.joystick_y = *adc_base;
   //data.slider_x = *adc_base;
   //data.slider_y = *adc_base;

   printf("ADC Readings - Joystick X: %d, Joystick Y: %d, Slider X: %d, Slider Y: %d\r\n",
           *adc_base, *adc_base,
           *adc_base, *adc_base);

   return data;
}