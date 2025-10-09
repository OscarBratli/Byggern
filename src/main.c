#include <util/delay.h>
#include "cpu_time/cpu_time.h"
#include "spi/spi.h"
#include "oled/oled.h"

void setup(void)
{
    // Initialize SPI for OLED communication
    spi_setup();
    
    // Initialize OLED and display hello world
    oled_init();
    oled_clear_screen();
    oled_print_string("Hello World!", 24, 3);  // Centered on screen
}

void loop(void)
{
    // Main loop - Hello World stays displayed
    _delay_ms(1000);
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
 