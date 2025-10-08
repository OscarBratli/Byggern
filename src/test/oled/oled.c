#include "oled.h"

void oled_test_setup(void)
{
    uart_init(MYUBRR);
    printf("OLED Test Setup\r\n");
    oled_init();
    oled_print("Starting ...");
    _delay_ms(2000);
}

void oled_test_loop(void)
{

    printf("OLED Test Loop\r\n");
    oled_clear();
    oled_print("OLED Test 1");
    _delay_ms(2000);

    oled_clear();
    oled_print("OLED Test 2");
    _delay_ms(2000);
}
