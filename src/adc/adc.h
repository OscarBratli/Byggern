#pragma once

#include <stdint.h>
typedef struct  
{
    uint8_t joystick_x;
    uint8_t joystick_y;
    uint8_t slider_x;
    uint8_t slider_y;
} ADC_values_t;

// MAX156 ADC driver functions (based on working repository)
void adc_init(void);
ADC_values_t adc_read();              // Method 1: Direct channel write
