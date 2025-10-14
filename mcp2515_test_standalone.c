#include <util/delay.h>
#include <stdio.h>
#include <avr/io.h>
#include "spi/spi.h"
#include "uart/uart.h"
#include "CANBUS/mcp2515.h"

// Simple MCP2515 test program
int main(void) {
    // Initialize UART for output
    uart_init(MYUBRR);
    printf("\r\n=== MCP2515 Standalone Test ===\r\n");
    
    // Initialize and test MCP2515
    mcp2515_init();
    _delay_ms(50);
    
    mcp2515_reset();
    _delay_ms(50);
    
    // Test basic connection
    if (mcp2515_test_connection()) {
        printf("MCP2515: OK\r\n");
        
        // Set loopback mode
        mcp2515_init_loopback();
        uint8_t mode = mcp2515_read(MCP_CANSTAT) & MODE_MASK;
        printf("Mode: 0x%02X\r\n", mode);
        
        if (mode == MODE_LOOPBACK) {
            printf("Loopback: OK\r\n");
            
            // Quick loopback test
            printf("Testing message...\r\n");
            
            // Load simple message
            mcp2515_write(0x31, 0x02);  // SIDH
            mcp2515_write(0x32, 0x60);  // SIDL  
            mcp2515_write(0x35, 0x02);  // DLC = 2 bytes
            mcp2515_write(0x36, 0xAA);  // Data 0
            mcp2515_write(0x37, 0x55);  // Data 1
            
            // Send message
            mcp2515_request_to_send(MCP_RTS_TX0);
            _delay_ms(10);
            
            // Check reception
            uint8_t intf = mcp2515_read(MCP_CANINTF);
            if (intf & MCP_RX0IF) {
                printf("Message received!\r\n");
                uint8_t d0 = mcp2515_read(MCP_RXB0SIDH + 5);
                uint8_t d1 = mcp2515_read(MCP_RXB0SIDH + 6);
                printf("Data: 0x%02X 0x%02X\r\n", d0, d1);
            } else {
                printf("No message received\r\n");
            }
        } else {
            printf("Loopback: FAIL\r\n");
        }
    } else {
        printf("MCP2515: FAIL\r\n");
    }
    
    printf("=== Test Complete ===\r\n");
    
    // Loop forever
    while(1) {
        _delay_ms(1000);
    }
    
    return 0;
}