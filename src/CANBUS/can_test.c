// CAN driver unit tests - can be run without hardware
#include "can.h"
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>

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