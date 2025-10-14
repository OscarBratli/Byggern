#ifndef MCP2515_TEST_H
#define MCP2515_TEST_H

// Test functions for MCP2515 driver
void mcp2515_test_basic_functions(void);      // Test basic read/write operations
void mcp2515_test_loopback_message(void);     // Test sending/receiving in loopback mode
void mcp2515_run_all_tests(void);             // Run all tests

#endif