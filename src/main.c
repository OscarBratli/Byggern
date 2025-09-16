#include <stdio.h>
#include <stdlib.h>
#include "uart/uart.h"

#include "test/sram/sram.h"
// #include "test/echo/echo.h"

void setup(void) {
    uart_init(MYUBRR);
    test_sram();
}

void loop(void) {
}

int main(void)
{
    setup();

    while (1)
{
        loop();
    }

    return 0;
}
