#include <stdio.h>
#include <stdlib.h>
#include "uart/uart.h"
#include "cpu_time/cpu_time.h"

#include "test/sram/sram.h"
#include "test/echo/echo.h"
#include "test/joystick/joystick.h"

void setup(void)
{
    // sram_test_setup();
    // echo_test_setup();
    joystick_test_setup();
}

void loop(void)
{
    // sram_test_loop();
    // echo_test_loop();
    joystick_test_loop();
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
