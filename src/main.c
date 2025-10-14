#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include "spi/spi.h"
#include "ioboard/ioboard.h"
#include <avr/pgmspace.h>
#include "uart/uart.h"
#include "oled/oled.h"
#include "joystick/joystick.h"
#include "adc/adc.h"
#include "menu/menu.h"
#include "CANBUS/mcp2515.h"
#include "CANBUS/mcp2515_test.h"
#include "CANBUS/can.h"
#include "CANBUS/can_test.h"

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
    printf_P(PSTR("MCP2515 test starting...\r\n"));
    
    mcp2515_init();
    _delay_ms(50);
    mcp2515_reset();
    _delay_ms(50);
    
    uint8_t canstat = mcp2515_read(MCP_CANSTAT);
    printf_P(PSTR("CANSTAT: 0x%02X\r\n"), canstat);
    
    if (mcp2515_test_connection()) {
        printf_P(PSTR("MCP2515: OK\r\n"));
    } else {
        printf_P(PSTR("MCP2515: FAIL\r\n"));
    }
}

// Task 4: Test CAN communication with loopback
void test_can_loopback(void) {
    printf_P(PSTR("\r\n=== Task 4: CAN Communication Test ===\r\n"));
    
    // Initialize CAN controller in loopback mode
    can_init();
    _delay_ms(100);
    
    // Test message 1
    can_message_t test_msg = {
        .id = 0x123,
        .length = 4,
        .data = {0xDE, 0xAD, 0xBE, 0xEF}
    };
    
    printf_P(PSTR("Sending CAN message: ID=0x%03X, Len=%d, Data="), test_msg.id, test_msg.length);
    for (uint8_t i = 0; i < test_msg.length; i++) {
        printf_P(PSTR("0x%02X "), test_msg.data[i]);
    }
    printf_P(PSTR("\r\n"));
    
    // Send message
    if (can_send_message(&test_msg)) {
        printf_P(PSTR("Message sent successfully\r\n"));
    } else {
        printf_P(PSTR("Failed to send message\r\n"));
        return;
    }
    
    // Wait a bit for loopback
    _delay_ms(10);
    
    // Try to receive the message back
    if (can_message_pending()) {
        can_message_t received_msg;
        if (can_receive_message(&received_msg)) {
            printf_P(PSTR("Received CAN message: ID=0x%03X, Len=%d, Data="), received_msg.id, received_msg.length);
            for (uint8_t i = 0; i < received_msg.length; i++) {
                printf_P(PSTR("0x%02X "), received_msg.data[i]);
            }
            printf_P(PSTR("\r\n"));
            
            // Verify data matches
            bool match = (test_msg.id == received_msg.id) && 
                        (test_msg.length == received_msg.length);
            if (match) {
                for (uint8_t i = 0; i < test_msg.length; i++) {
                    if (test_msg.data[i] != received_msg.data[i]) {
                        match = false;
                        break;
                    }
                }
            }
            
            if (match) {
                printf_P(PSTR("✓ CAN loopback test PASSED!\r\n"));
            } else {
                printf_P(PSTR("✗ CAN loopback test FAILED - data mismatch\r\n"));
            }
        } else {
            printf_P(PSTR("✗ Failed to receive message\r\n"));
        }
    } else {
        printf_P(PSTR("✗ No message pending in receive buffer\r\n"));
    }
    
    printf_P(PSTR("=== CAN Test Complete ===\r\n\r\n"));
}

static uint8_t mcp2515_test_run = 0;
static uint8_t can_test_run = 0;
static uint8_t can_unit_test_run = 0;

void loop(void)
{
    // Run MCP2515 test once after startup
    if (!mcp2515_test_run) {
        _delay_ms(2000);  // Wait 2 seconds after startup
        test_mcp2515();
        mcp2515_test_run = 1;
    }
    
    // Run CAN unit tests first (doesn't require hardware)
    if (mcp2515_test_run && !can_unit_test_run) {
        _delay_ms(500);
        test_can_message_structure();
        test_can_edge_cases();
        can_unit_test_run = 1;
    }
    
    // Run CAN loopback test after unit tests
    if (can_unit_test_run && !can_test_run) {
        _delay_ms(1000);  // Wait 1 second between tests
        test_can_loopback();
        can_test_run = 1;
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