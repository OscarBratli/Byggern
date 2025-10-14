#include "mcp2515_test.h"
#include "mcp2515.h"
#include "../uart/uart.h"
#include <stdio.h>
#include <util/delay.h>

void mcp2515_test_basic_functions(void) {
    printf("=== MCP2515 Test ===\r\n");
    
    // Initialize driver
    mcp2515_init();
    _delay_ms(10);
    
    // Reset and test connection
    mcp2515_reset();
    _delay_ms(10);
    
    if (mcp2515_test_connection()) {
        printf("MCP2515 OK!\r\n");
    } else {
        printf("MCP2515 FAIL!\r\n");
        return;
    }
    
    // Test loopback mode
    mcp2515_init_loopback();
    uint8_t canstat = mcp2515_read(MCP_CANSTAT);
    printf("Mode: 0x%02X\r\n", canstat & MODE_MASK);
    
    printf("Test Complete\r\n");
}

void mcp2515_test_loopback_message(void) {
    printf("=== Loopback Test ===\r\n");
    
    mcp2515_init_loopback();
    
    // Load simple message in TX buffer 0
    mcp2515_write(0x31, 0x02);  // SIDH
    mcp2515_write(0x32, 0x60);  // SIDL  
    mcp2515_write(0x35, 0x02);  // DLC = 2 bytes
    mcp2515_write(0x36, 0xAA);  // Data byte 0
    mcp2515_write(0x37, 0x55);  // Data byte 1
    
    printf("Msg loaded\r\n");
    
    // Send message
    mcp2515_request_to_send(MCP_RTS_TX0);
    _delay_ms(10);
    
    // Check if received
    uint8_t canintf = mcp2515_read(MCP_CANINTF);
    if (canintf & MCP_RX0IF) {
        printf("Loopback OK!\r\n");
        uint8_t data0 = mcp2515_read(MCP_RXB0SIDH + 5);
        uint8_t data1 = mcp2515_read(MCP_RXB0SIDH + 6);
        printf("RX: 0x%02X 0x%02X\r\n", data0, data1);
        
        // Clear interrupt flag
        mcp2515_bit_modify(MCP_CANINTF, MCP_RX0IF, 0x00);
    } else {
        printf("Loopback FAIL\r\n");
    }
}

void mcp2515_run_all_tests(void) {
    mcp2515_test_basic_functions();
    _delay_ms(100);
    mcp2515_test_loopback_message();
}