#include "can.h"

void can_test_setup(void) 
{
    uart_init(MYUBRR);
    spi_setup();
}

// Test CAN message structure and ID handling
void test_can_message_structure(void) {
    printf_P(PSTR("\r\n--- CAN Struct Test ---\r\n"));
    
    // Test basic message structure
    can_message_t test_msg = {
        .id = 0x123,
        .length = 4,
        .data = {0xDE, 0xAD, 0xBE, 0xEF}
    };
    
    printf_P(PSTR("Msg: ID=0x%03X Len=%d\r\n"), test_msg.id, test_msg.length);
    
    // Test ID bit manipulation (simulate what MCP2515 does)
    uint8_t sidh = (test_msg.id >> 3) & 0xFF;     // ID[10:3]
    uint8_t sidl = (test_msg.id << 5) & 0xE0;     // ID[2:0] shifted to bits 7:5
    
    // Reconstruct ID (simulate receive)
    uint16_t reconstructed_id = ((uint16_t)sidh << 3) | ((sidl >> 5) & 0x07);
    
    if (reconstructed_id == test_msg.id) {
        printf_P(PSTR("ID encode/decode: OK\r\n"));
    } else {
        printf_P(PSTR("ID encode/decode: FAIL\r\n"));
    }
    
    printf_P(PSTR("--- Test Complete ---\r\n\r\n"));
}

// Test edge cases
void test_can_edge_cases(void) {
    printf_P(PSTR("--- CAN Edge Cases ---\r\n"));
    
    // Test maximum ID (11-bit)
    uint16_t max_id = 0x7FF;  // 11-bit max
    uint8_t sidh = (max_id >> 3) & 0xFF;
    uint8_t sidl = (max_id << 5) & 0xE0;
    uint16_t reconstructed = ((uint16_t)sidh << 3) | ((sidl >> 5) & 0x07);
    
    printf_P(PSTR("Max ID (0x7FF): %s\r\n"), 
             (reconstructed == max_id) ? "OK" : "FAIL");
    
    // Test minimum ID
    uint16_t min_id = 0x000;
    sidh = (min_id >> 3) & 0xFF;
    sidl = (min_id << 5) & 0xE0;
    reconstructed = ((uint16_t)sidh << 3) | ((sidl >> 5) & 0x07);
    
    printf_P(PSTR("Min ID (0x000): %s\r\n"), 
             (reconstructed == min_id) ? "OK" : "FAIL");
    
    printf_P(PSTR("--- Edge Tests Done ---\r\n\r\n"));
}

void mcp2515_test_basic_functions(void) {
    printf_P(PSTR("=== MCP2515 Test ===\r\n"));
    
    // Initialize driver
    mcp2515_init();
    _delay_ms(10);
    
    // Reset and test connection
    mcp2515_reset();
    _delay_ms(10);
    
    if (mcp2515_test_connection()) {
        printf_P(PSTR("MCP2515 OK!\r\n"));
    } else {
        printf_P(PSTR("MCP2515 FAIL!\r\n"));
        return;
    }
    
    // Test loopback mode
    mcp2515_init_loopback();
    uint8_t canstat = mcp2515_read(MCP_CANSTAT);
    printf_P(PSTR("Mode: 0x%02X\r\n"), canstat & MODE_MASK);
    
    printf_P(PSTR("Test Complete\r\n"));
}

void mcp2515_test_loopback_message(void) {
    printf_P(PSTR("=== Loopback Test ===\r\n"));
    
    mcp2515_init_loopback();
    
    // Load simple message in TX buffer 0
    mcp2515_write(0x31, 0x02);  // SIDH
    mcp2515_write(0x32, 0x60);  // SIDL  
    mcp2515_write(0x35, 0x02);  // DLC = 2 bytes
    mcp2515_write(0x36, 0xAA);  // Data byte 0
    mcp2515_write(0x37, 0x55);  // Data byte 1
    
    printf_P(PSTR("Msg loaded\r\n"));
    
    // Send message
    mcp2515_request_to_send(MCP_RTS_TX0);
    _delay_ms(10);
    
    // Check if received
    uint8_t canintf = mcp2515_read(MCP_CANINTF);
    if (canintf & MCP_RX0IF) {
        printf_P(PSTR("Loopback OK!\r\n"));
        uint8_t data0 = mcp2515_read(MCP_RXB0SIDH + 5);
        uint8_t data1 = mcp2515_read(MCP_RXB0SIDH + 6);
        printf_P(PSTR("RX: 0x%02X 0x%02X\r\n"), data0, data1);
        
        // Clear interrupt flag
        mcp2515_bit_modify(MCP_CANINTF, MCP_RX0IF, 0x00);
    } else {
        printf_P(PSTR("Loopback FAIL\r\n"));
    }
}

void mcp2515_run_all_tests(void) {
    mcp2515_test_basic_functions();
    _delay_ms(100);
    mcp2515_test_loopback_message();
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

// Continuous loopback test - runs forever
void test_can_loopback_continuous(void) {
    static uint8_t test_counter = 0;
    static uint32_t success_count = 0;
    static uint32_t fail_count = 0;
    
    // Create test message with incrementing data
    can_message_t test_msg = {
        .id = 0x200 + (test_counter & 0x0F),  // ID from 0x200-0x20F
        .length = 4,
        .data = {test_counter, test_counter + 1, test_counter + 2, test_counter + 3}
    };
    
    printf_P(PSTR("Test #%d: Send ID=0x%03X Data=[%02X %02X %02X %02X] "), 
             test_counter, test_msg.id, 
             test_msg.data[0], test_msg.data[1], test_msg.data[2], test_msg.data[3]);
    
    // Send message
    if (can_send_message(&test_msg)) {
        _delay_ms(5);  // Small delay for loopback
        
        // Check for received message
        if (can_message_pending()) {
            can_message_t received_msg;
            if (can_receive_message(&received_msg)) {
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
                    success_count++;
                    printf_P(PSTR("✓ OK\r\n"));
                } else {
                    fail_count++;
                    printf_P(PSTR("✗ MISMATCH - RX: ID=0x%03X Data=[%02X %02X %02X %02X]\r\n"), 
                             received_msg.id,
                             received_msg.data[0], received_msg.data[1], 
                             received_msg.data[2], received_msg.data[3]);
                }
            } else {
                fail_count++;
                printf_P(PSTR("✗ RX ERROR\r\n"));
            }
        } else {
            fail_count++;
            printf_P(PSTR("✗ NO RX\r\n"));
        }
    } else {
        fail_count++;
        printf_P(PSTR("✗ TX FAIL\r\n"));
    }
    
    test_counter++;
    
    // Print statistics every 10 tests
    if (test_counter % 10 == 0) {
        printf_P(PSTR("--- Stats: Success=%lu, Fail=%lu, Rate="), success_count, fail_count);
        if (success_count + fail_count > 0) {
            uint16_t rate = (success_count * 100) / (success_count + fail_count);
            printf_P(PSTR("%d%%"), rate);
        } else {
            printf_P(PSTR("N/A"));
        }
        printf_P(PSTR(" ---\r\n\r\n"));
    }
    
    _delay_ms(100);  // 100ms between tests for readability
}

static uint8_t mcp2515_test_run = 0;
static uint8_t can_test_run = 0;
static uint8_t can_unit_test_run = 0;

void can_test_loop(void)
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
    // menu_selector(); 
    //display_joystick();
}

// Alternative test loop that runs continuous loopback
void can_test_loop_continuous(void)
{
    // Run MCP2515 test once after startup
    if (!mcp2515_test_run) {
        _delay_ms(2000);  // Wait 2 seconds after startup
        printf_P(PSTR("\r\n=== Starting Continuous CAN Loopback Test ===\r\n"));
        test_mcp2515();
        mcp2515_test_run = 1;
        
        // Initialize CAN for loopback testing
        can_init();
        printf_P(PSTR("CAN initialized for continuous loopback testing\r\n"));
        printf_P(PSTR("Press reset to stop test\r\n\r\n"));
        return;
    }
    
    // Run continuous loopback test
    test_can_loopback_continuous();
}