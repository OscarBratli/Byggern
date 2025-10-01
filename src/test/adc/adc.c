#include "adc.h"

// Function to read from MAX156 ADC
int adc_read(int channel)
{
    // TODO: Implement SPI communication with MAX156
    // This is a placeholder - you need to implement SPI read
    // For now, return a test value that changes
    static int test_value = 100;
    test_value += (channel * 50) + (test_value % 100);
    return test_value % 4096;  // 12-bit ADC range
}

void adc_test_setup(void)
{
    uart_init(MYUBRR);
    printf("Startup OK\r\n");
}

void adc_test_loop(void)
{
    while (1)
    {
        int adc_value = adc_read(0);  // Read from ADC channel 0
        printf("ADC Value: %d\r\n", adc_value);
        _delay_ms(500);
        
        int adc_value2 = adc_read(1);  // Read from ADC channel 1
        printf("ADC Value Ch1: %d\r\n", adc_value2);
        _delay_ms(500);
    }
}
