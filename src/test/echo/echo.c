#include "echo.h"

void echo_test_setup(void)
{
    uart_init(MYUBRR);
    printf("Startup OK\r\n");
}

void echo_test_loop(void)
{
    while (1)
    {
        char c = getchar();

        printf("You typed: %c\r\n", c);

        if (c == 'q')
        {
            break;
        }
    }
}
