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
#include "CANBUS/mcp2515.h"
#include "CANBUS/mcp2515_test.h"

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


// Simple MCP2515 test function that can be called
void test_mcp2515(void) {
    printf("MCP2515 test starting...\r\n");
    
    mcp2515_init();
    _delay_ms(50);
    mcp2515_reset();
    _delay_ms(50);
    
    uint8_t canstat = mcp2515_read(MCP_CANSTAT);
    printf("CANSTAT: 0x%02X\r\n", canstat);
    
    if (mcp2515_test_connection()) {
        printf("MCP2515: OK\r\n");
    } else {
        printf("MCP2515: FAIL\r\n");
    }
}

static uint8_t mcp2515_test_run = 0;

void loop(void)
{
    // Run MCP2515 test once after startup
    if (!mcp2515_test_run) {
        _delay_ms(2000);  // Wait 2 seconds after startup
        test_mcp2515();
        mcp2515_test_run = 1;
    }
    
    // The menu system handles display and navigation internally
    menu_selector(); 
    //display_joystick();
}

int main(void)
{
    setup();
    
    while (1) {
        loop();
    }
    return 0; 
} 