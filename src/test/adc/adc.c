#include "adc.h"

void adc_test_setup(void)
{
    uart_init(MYUBRR);
    adc_init();
    printf_P(PSTR("Startup OK\r\n"));
}

void adc_test_loop(void)
{
    int adc0_value = adc_read(0); // Read from ADC channel 0
    int adc1_value = adc_read(1); // Read from ADC channel 1
    int adc2_value = adc_read(2); // Read from ADC channel 2
    int adc3_value = adc_read(3); // Read from ADC channel 3

    printf_P(PSTR("ADC Readings - Joystick X: %03d, Joystick Y: %03d, Slider X: %03d, Slider Y: %03d\r\n"),
           adc0_value, adc1_value,
           adc2_value, adc3_value);

    _delay_ms(10);
}
