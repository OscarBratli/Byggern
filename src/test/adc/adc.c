#include "adc.h"

void adc_test_setup(void)
{
    uart_init(MYUBRR);
    printf("Startup OK\r\n");
}

void adc_test_loop(void)
{
    while (1)
    {
        int adc_value = 0;
        printf("ADC Value: %d\r\n", adc_value);
        _delay_ms(500);
    }
}
