#include <stdio.h>
#include <stdlib.h>
#include "uart/uart.h"
#include "cpu_time/cpu_time.h"

#include "test/adc/adc.h"
#include "test/sram/sram.h"
#include "test/echo/echo.h"
#include "test/joystick/joystick.h"


// === UART config ===
#define F_CPU 4915200UL  // 4.9152 MHz crystal atmega162
#define BAUD 9600       // 9600 baud rate 
#define MYUBRR (F_CPU / 16 / BAUD - 1) //Calculate MYUBRR value for given F_CPU and BAUD rate should be 31 for 4.9152MHz and 9600 baud rate




void setup(void)
{
    // sram_test_setup();
    // echo_test_setup();
    // joystick_test_setup();
    adc_test_setup();
}

void loop(void)
{
    // sram_test_loop();
    // echo_test_loop();
    // joystick_test_loop();
    adc_test_loop();
}

int main(void)
{
    cpu_time_init();
    setup();

    while (1)
    {
        loop();
    }

    return 0;
}
