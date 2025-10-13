#include <util/delay.h>
#include <stdio.h>
#include "cpu_time/cpu_time.h"
#include "spi/spi.h"
#include "oled/oled.h"
#include "joystick/joystick.h"
#include "adc/adc.h"
#include "test/adc/adc.h"
#include "test/sram/sram.h"
#include "uart/uart.h"

void setup(void)
{
/**********************************************/
    // Initialize these for testing
    uart_init(MYUBRR);
    xmem_init();  
    adc_init();
    spi_setup();
    oled_init();
/**********************************************/
    adc_test_setup();
    oled_clear_screen();
    oled_print_string("Joystick Test", 0, 0);
}

void loop(void)
{
    display_joystick();
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
 