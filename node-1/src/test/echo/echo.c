#include "echo.h"

// === UART config ===
#define F_CPU 4915200UL  // 4.9152 MHz crystal atmega162
#define BAUD 9600       // 9600 baud rate 
#define MYUBRR (F_CPU / 16 / BAUD - 1) //Calculate MYUBRR value for given F_CPU and BAUD rate should be 31 for 4.9152MHz and 9600 baud rate


void echo_test_setup(void)
{
    uart_init(MYUBRR);
    printf_P(PSTR("Startup OK\r\n"));
}

void echo_test_loop(void)
{
    while (1)
    {
        char c = getchar();

        printf_P(PSTR("You typed: %c\r\n"), c);

        if (c == 'q')
        {
            break;
        }
    }
}
