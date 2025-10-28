#include "mcp2515.h"

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
    
    // Configure bit timing for 250kbps (with 16MHz crystal)
    // Bit rate calculation for MCP2515:
    // - TQ (Time Quantum) = 2 * Tosc * (BRP + 1)
    // - Bit Time = (SyncSeg + PropSeg + PS1 + PS2) * TQ
    // - For 250 kbps with 16 MHz: Bit Time = 4 µs
    //
    // Configuration: BRP=1, 16 TQ per bit (MUST MATCH NODE 2!)
    // - SyncSeg = 1 TQ (fixed)
    // - PropSeg = 6 TQ (PRSEG register value = 5, actual = 6)
    // - PS1 = 5 TQ (PHSEG1 register value = 4, actual = 5)
    // - PS2 = 4 TQ (PHSEG2 register value = 3, actual = 4)
    // - Total = 1 + 6 + 5 + 4 = 16 TQ
    // - TQ = 2 * (1/16MHz) * (1+1) = 0.25 µs
    // - Bit time = 16 * 0.25µs = 4µs = 250kbps ✓
    // - Sample point = (1 + 6 + 5) / 16 = 75%
    
    mcp2515_write(MCP_CNF1, 0x01);  // SJW=1 (2TQ), BRP=1
    mcp2515_write(MCP_CNF2, 0xB5);  // BTLMODE=1, SAM=0, PHSEG1=5, PRSEG=6 (0xB5 = 10110101)
    mcp2515_write(MCP_CNF3, 0x03);  // SOF=0, WAKFIL=0, PHSEG2=4 (0x03 = 00000011)
    
    // Configure RX buffer 0 to accept ALL messages (turn off filters)
    // RXB0CTRL: Accept all messages (standard + extended), rollover enabled
    mcp2515_write(MCP_RXB0CTRL, 0x60);  // RXM[1:0] = 11 (turn off filters, accept all)
    
    // Configure RX buffer 1 to accept ALL messages
    mcp2515_write(MCP_RXB1CTRL, 0x60);  // RXM[1:0] = 11 (turn off filters, accept all)
    
    // Set to loopback mode for testing
    mcp2515_set_mode(MODE_LOOPBACK);
}

// Initialize MCP2515 in NORMAL mode for actual CAN bus communication
void mcp2515_init_normal(void) {
    // Initialize the driver
    mcp2515_init();
    
    // Reset the MCP2515
    mcp2515_reset();
    
    // Set to configuration mode first
    mcp2515_set_mode(MODE_CONFIG);
    
    // Configure bit timing for 250kbps (same as loopback - with 16MHz crystal)
    // MUST MATCH LOOPBACK MODE CONFIGURATION ABOVE
    mcp2515_write(MCP_CNF1, 0x01);  // SJW=1 (2TQ), BRP=1
    mcp2515_write(MCP_CNF2, 0xB5);  // BTLMODE=1, SAM=0, PHSEG1=5, PRSEG=6
    mcp2515_write(MCP_CNF3, 0x03);  // SOF=0, WAKFIL=0, PHSEG2=4
    
    // Configure RX buffer 0 to accept ALL messages (turn off filters)
    mcp2515_write(MCP_RXB0CTRL, 0x60);  // RXM[1:0] = 11 (turn off filters, accept all)
    
    // Configure RX buffer 1 to accept ALL messages
    mcp2515_write(MCP_RXB1CTRL, 0x60);  // RXM[1:0] = 11 (turn off filters, accept all)
    
    // Set to NORMAL mode for real CAN bus communication
    mcp2515_set_mode(MODE_NORMAL);
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