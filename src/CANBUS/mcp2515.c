#include "mcp2515.h"
#include "../spi/spi.h"
#include "../uart/uart.h"
#include <util/delay.h>
#include <avr/io.h>
#include <stdio.h>

// MCP2515 Chip Select pin - based on wiring tables (PE0)
#define MCP2515_CS PE0

// Helper function to select MCP2515
static void mcp2515_select(void) {
    PORTE &= ~(1 << MCP2515_CS);  // Pull CS low
}

// Helper function to deselect MCP2515  
static void mcp2515_deselect(void) {
    PORTE |= (1 << MCP2515_CS);   // Pull CS high
}

// Initialize MCP2515 driver (call this before using other functions)
void mcp2515_init(void) {
    // Initialize SPI first
    SPI_MasterInit();
    
    // Set MCP2515 CS pin as output and deselect
    DDRE |= (1 << MCP2515_CS);
    mcp2515_deselect();
    
    // Small delay to ensure MCP2515 is ready
    _delay_ms(10);
}

// Reset the MCP2515
void mcp2515_reset(void) {
    mcp2515_select();
    SPI_Transfer(MCP_RESET);
    mcp2515_deselect();
    
    // Wait for reset to complete
    _delay_ms(10);
}

// Read a register from MCP2515
uint8_t mcp2515_read(uint8_t address) {
    uint8_t result;
    
    mcp2515_select();
    SPI_Transfer(MCP_READ);      // Send read command
    SPI_Transfer(address);       // Send register address
    result = SPI_Transfer(0x00); // Read data (send dummy byte)
    mcp2515_deselect();
    
    return result;
}

// Write to a register in MCP2515
void mcp2515_write(uint8_t address, uint8_t data) {
    mcp2515_select();
    SPI_Transfer(MCP_WRITE);     // Send write command
    SPI_Transfer(address);       // Send register address
    SPI_Transfer(data);          // Send data
    mcp2515_deselect();
}

// Request to send (RTS) - trigger transmission of a message buffer
void mcp2515_request_to_send(uint8_t buffer_select) {
    mcp2515_select();
    SPI_Transfer(buffer_select); // Send RTS command for specific buffer
    mcp2515_deselect();
}

// Read status register
uint8_t mcp2515_read_status(void) {
    uint8_t result;
    
    mcp2515_select();
    SPI_Transfer(MCP_READ_STATUS); // Send read status command
    result = SPI_Transfer(0x00);   // Read status
    mcp2515_deselect();
    
    return result;
}

// Bit modify - modify specific bits in a register
void mcp2515_bit_modify(uint8_t address, uint8_t mask, uint8_t data) {
    mcp2515_select();
    SPI_Transfer(MCP_BITMOD);    // Send bit modify command
    SPI_Transfer(address);       // Send register address
    SPI_Transfer(mask);          // Send mask (which bits to modify)
    SPI_Transfer(data);          // Send new data
    mcp2515_deselect();
}

// Set MCP2515 to specified mode
void mcp2515_set_mode(uint8_t mode) {
    mcp2515_bit_modify(MCP_CANCTRL, MODE_MASK, mode);
    
    // Wait for mode change to complete
    uint8_t timeout = 100;
    while (timeout-- > 0) {
        uint8_t current_mode = mcp2515_read(MCP_CANSTAT) & MODE_MASK;
        if (current_mode == mode) {
            break;
        }
        _delay_ms(1);
    }
}

// Initialize MCP2515 for loopback mode (for testing)
void mcp2515_init_loopback(void) {
    // Initialize the driver
    mcp2515_init();
    
    // Reset the MCP2515
    mcp2515_reset();
    
    // Set to configuration mode first
    mcp2515_set_mode(MODE_CONFIG);
    
    // Configure bit timing for 125kbps (assuming 16MHz crystal)
    // These values are typical for 125kbps with 16MHz crystal
    mcp2515_write(MCP_CNF1, 0x03);  // SJW=1, BRP=3
    mcp2515_write(MCP_CNF2, 0xB1);  // BTLMODE=1, SAM=0, PHSEG1=4, PRSEG=1  
    mcp2515_write(MCP_CNF3, 0x05);  // PHSEG2=5
    
    // Set to loopback mode for testing
    mcp2515_set_mode(MODE_LOOPBACK);
}

// Check if MCP2515 is connected and responding
uint8_t mcp2515_test_connection(void) {
    // Try to write and read back from a known register
    uint8_t test_value = 0x55;
    
    // Set to config mode first (allows writing to most registers)
    mcp2515_set_mode(MODE_CONFIG);
    
    // Write test value to CNF1 register
    mcp2515_write(MCP_CNF1, test_value);
    
    // Read it back
    uint8_t read_value = mcp2515_read(MCP_CNF1);
    
    return (read_value == test_value) ? 1 : 0;
}