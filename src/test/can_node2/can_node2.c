#include "can_node2.h"

static uint8_t message_counter = 0;

void can_node2_test_setup(void) 
{
    // Disable watchdog timer immediately
    wdt_disable();
    
    // Initialize UART for debugging
    uart_init(MYUBRR);
    printf_P(PSTR("\r\n=== Node 1 to Node 2 CAN Test ===\r\n"));
    
    // Initialize MCP2515 in normal mode - simplified without step-by-step debug
    mcp2515_init();
    mcp2515_reset();
    
    // Small delay after reset
    for(volatile uint16_t i = 0; i < 10000; i++);
    
    mcp2515_set_mode(MODE_CONFIG);
    
    // Configure bit timing
    mcp2515_write(MCP_CNF1, 0x03);
    mcp2515_write(MCP_CNF2, 0xFA);
    mcp2515_write(MCP_CNF3, 0x05);
    
    // Disable RX filters
    mcp2515_write(MCP_RXB0CTRL, 0x60);
    mcp2515_write(MCP_RXB1CTRL, 0x60);
    
    mcp2515_set_mode(MODE_NORMAL);
    
    printf_P(PSTR("CAN initialized! Ready to send messages\r\n"));
    printf_P(PSTR("Bit rate: 125kbps\r\n"));
    printf_P(PSTR("Setup complete - entering main loop\r\n\r\n"));
}

void can_node2_test_loop(void)
{
    static uint8_t loop_count = 0;
    
    // Print every time we enter the loop
    printf_P(PSTR("Loop iteration %d\r\n"), loop_count++);
    
    // Create a test message
    can_message_t msg = {
        .id = 0x123,
        .length = 8,
        .data = {
            0x10 + message_counter,  // Data byte 0
            0x20 + message_counter,  // Data byte 1
            0x30 + message_counter,  // Data byte 2
            0x40 + message_counter,  // Data byte 3
            0x50,                     // Data byte 4
            0x60,                     // Data byte 5
            0x70,                     // Data byte 6
            0x80                      // Data byte 7
        }
    };
    
    printf_P(PSTR("Sending message #%d to Node 2:\r\n"), message_counter);
    printf_P(PSTR("  ID: 0x%03X\r\n"), msg.id);
    printf_P(PSTR("  Length: %d\r\n"), msg.length);
    printf_P(PSTR("  Data: "));
    for (uint8_t i = 0; i < msg.length; i++) {
        printf_P(PSTR("0x%02X "), msg.data[i]);
    }
    printf_P(PSTR("\r\n"));
    
    // Send the message
    if (can_send_message(&msg)) {
        printf_P(PSTR("✓ Message sent successfully\r\n\r\n"));
        message_counter++;
    } else {
        printf_P(PSTR("✗ Failed to send message\r\n\r\n"));
    }
    
    // Wait 1 second before sending next message
    for(volatile uint32_t i = 0; i < 500000; i++);
}
