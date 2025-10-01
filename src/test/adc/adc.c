#include "adc.h"

void adc_test_setup(void)
{
    uart_init(MYUBRR);
    adc_init();
    printf("Startup OK\r\n");
}

void adc_test_loop(void)
{
    int adc0_value = adc_read(0);  // Read from ADC channel 0
    int adc2_value = adc_read(1); // Read from ADC channel 1
    printf("ADC Value: %.3d, %.3d\r\n", adc0_value, adc2_value);
    _delay_ms(100);
}
