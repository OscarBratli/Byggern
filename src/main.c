#include <stdio.h>
#include <stdlib.h>
#include "uart/uart.h"
#include "cpu_time/cpu_time.h"

#include "test/adc/adc.h"
#include "test/sram/sram.h"
//#include "test/echo/echo.h"
//#include "test/oled/oled.h"
#include "test/joystick/joystick.h"
#include "spi/spi.h"

void setup(void)
{
    sram_test_setup();
    // echo_test_setup();
    // joystick_test_setup();
    // adc_test_setup();
    //oled_test_setup();
     spi_setup();
 
   
}

void loop(void)
{
    spi_loop();
    //sram_test_loop();
    // echo_test_loop();
    // joystick_test_loop();
    // adc_test_loop();
    //oled_test_loop();
    
}

int main(void)
{
    cpu_time_init(); 
    setup();
   // loop();
   while (1)
    {
        loop();
    }
    return 0; 
    
}
 