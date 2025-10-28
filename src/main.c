#include "test/can/can.h"
#include "test/adc/adc.h"
#include "test/echo/echo.h"
#include "test/sram/sram.h"
#include "test/menu/menu.h"

void setup(void) 
{
     can_test_setup();
    // adc_test_setup();
    // echo_test_setup();
    // sram_test_setup();
     menu_test_setup();
}

void loop(void)
{
    //can_test_loop_continuous();  // Loopback test only
    can_test_loop_node2();         // Node 1 <-> Node 2 communication
    // adc_test_loop();
    // echo_test_loop();
    // sram_test_loop();
    menu_test_loop();
}

int main(void)
{
    setup();  
    
    while (1) {
        loop(); 
        _delay_ms(100);
    }
    return 0; 
}
