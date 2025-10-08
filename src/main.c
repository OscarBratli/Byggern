#include <stdio.h>
#include <stdlib.h>
#include "uart/uart.h"
#include "cpu_time/cpu_time.h"

#include "test/sram/sram.h"
#include "test/echo/echo.h"
#include "test/joystick/joystick.h"

void setup(void)
{
    uart_init(MYUBRR);
    xmem_init();
    adc_init();
    // Uncomment the test you want to run:
    sram_test_setup();
    // echo_test_setup();
    // joystick_test_setup();
    
}

void loop(void)
{
    // Uncomment the corresponding test loop:
    sram_test_loop();
    // echo_test_loop();
    // joystick_test_loop();

    //ADC_values_t adc_data = adc_read();
    
   //_delay_ms(500);  // Delay between readings
}

int main(void)
{
    //cpu_time_init();
    setup();
    /*
    while (1)
    {
        loop();
    }
    */
   loop();
    return 0;

}
