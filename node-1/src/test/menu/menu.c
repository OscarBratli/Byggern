#include "menu.h"

void menu_test_setup(void) 
{
    // Initialize hardware
    uart_init(MYUBRR);
    adc_init();
    joystick_init();
    spi_setup();
    oled_init();
    ioboard_init();
    
    // Initial display
    oled_clear_screen();
    display_menu();
}

void menu_test_loop(void)
{
    menu_selector();
}
