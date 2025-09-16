#include "echo.h"

void test_echo(void)
{
    printf("Startup OK\r\n");

    while (1)
    {
        char c = getchar();

        printf("You typed: %c\r\n", c);

        if (c == 'q') {
            break;
        }
    }
}
