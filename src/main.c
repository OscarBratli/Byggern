#include "test/can/can.h"
#include "test/can_node2/can_node2.h"
#include "test/adc/adc.h"
#include "test/echo/echo.h"
#include "test/sram/sram.h"
#include "test/menu/menu.h"
#include <avr/wdt.h>
#include "uart/uart.h"
#include <avr/pgmspace.h>
#include <stdio.h>

// Disable watchdog very early
void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));
void wdt_init(void)
{
    wdt_disable();
}

void setup(void) 
{
    // can_test_setup();       // Loopback test
    can_node2_test_setup();    // Node 1 to Node 2 communication test
    // adc_test_setup();
    // echo_test_setup();
    // sram_test_setup();
    // menu_test_setup();
    
    printf_P(PSTR("*** SETUP COMPLETE ***\r\n"));
}

void loop(void)
{
    printf_P(PSTR("*** LOOP CALLED ***\r\n"));
    // can_test_loop_continuous();  // Loopback test
    can_node2_test_loop();          // Node 1 to Node 2 communication test
    // can_test_loop();
    // adc_test_loop();
    // echo_test_loop();
    // sram_test_loop();
    // menu_test_loop();
}

int main(void)
{
    printf_P(PSTR("*** MAIN STARTED ***\r\n"));
    setup();
    printf_P(PSTR("*** ENTERING WHILE LOOP ***\r\n"));
    
    while (1) { 
        loop(); 
        // Small delay between loop iterations
        for(volatile uint32_t i = 0; i < 100000; i++);
    }
    return 0; 
}
