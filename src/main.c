#include <stdio.h>
#include <stdlib.h>
#include "uart/uart.h"

#include "test/echo/echo.h"

void setup(void) {
    uart_init(MYUBRR);
}

void loop(void) {
    test_echo();
}

int main(void)
{
    setup();

    while (1) {
        loop();
    }

    return 0;
}
