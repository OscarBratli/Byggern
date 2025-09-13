#include "uart.h"
#include <stdio.h>

int main(void) {
    uart_init(MYUBRR);

    printf("Startup OK\r\n");

    while (1) {
        char c = getchar();     // wait for key
        //uart_transmit(c);       // raw echo, test TX works
        printf("You typed: %c\r\n", c);   // formatted echo
    }
}
