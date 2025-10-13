#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include "spi/spi.h"
#include "ioboard/ioboard.h"
#include "uart/uart.h"
#include "oled/oled.h"
#include "joystick/joystick.h"
#include "adc/adc.h"
#include "menu/menu.h"

void setup(void)
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


void loop(void)
{
    // The menu system handles display and navigation internally
    menu_selector();
}

int main(void)
{
    setup();
    
    while (1) {
        loop();
    }
    return 0;
} 