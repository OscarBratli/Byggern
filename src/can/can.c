#include "can.h"

// Initialize CAN controller in normal mode for CAN bus communication
void can_init(void) {
    // Initialize MCP2515 in normal mode (ready for CAN bus communication)
    mcp2515_init_normal();
}

// Send a CAN message
uint8_t can_send_message(can_message_t* msg) {
    if (msg == 0 || msg->length > 8) {
        return 0; // Invalid message
    }
    
    // Use TX buffer 0 (starts at address 0x30)
    // Check if TX buffer is free
    uint8_t ctrl = mcp2515_read(MCP_TXB0CTRL);
    if (ctrl & 0x08) {  // TXREQ bit set means buffer is busy
        return 0; // Buffer busy
    }
    
    // Load message into TX buffer 0
    // Standard ID format: ID[10:3] goes to SIDH, ID[2:0] goes to SIDL[7:5]
    uint8_t sidh = (msg->id >> 3) & 0xFF;           // ID[10:3]
    uint8_t sidl = (msg->id << 5) & 0xE0;           // ID[2:0] shifted to bits 7:5
    
    mcp2515_write(0x31, sidh);                      // TXB0SIDH
    mcp2515_write(0x32, sidl);                      // TXB0SIDL  
    mcp2515_write(0x33, 0x00);                      // TXB0EID8 (not used for standard ID)
    mcp2515_write(0x34, 0x00);                      // TXB0EID0 (not used for standard ID)
    mcp2515_write(0x35, msg->length & 0x0F);        // TXB0DLC (data length)
    
    // Load data bytes
    for (uint8_t i = 0; i < msg->length; i++) {
        mcp2515_write(0x36 + i, msg->data[i]);      // TXB0D0-TXB0D7
    }
    
    // Request transmission
    mcp2515_request_to_send(MCP_RTS_TX0);
    
    return 1; // Success
}

// Receive a CAN message
uint8_t can_receive_message(can_message_t* msg) {
    if (msg == 0) {
        return 0; // Invalid pointer
    }
    
    // Check if message is available in RX buffer 0
    uint8_t canintf = mcp2515_read(MCP_CANINTF);
    if (!(canintf & MCP_RX0IF)) {
        return 0; // No message pending
    }
    
    // Read message from RX buffer 0 (starts at 0x61)
    uint8_t sidh = mcp2515_read(MCP_RXB0SIDH + 0);  // RXB0SIDH
    uint8_t sidl = mcp2515_read(MCP_RXB0SIDH + 1);  // RXB0SIDL
    uint8_t dlc = mcp2515_read(MCP_RXB0SIDH + 4);   // RXB0DLC
    
    // Reconstruct ID from SIDH and SIDL
    msg->id = ((uint16_t)sidh << 3) | ((sidl >> 5) & 0x07);
    msg->length = dlc & 0x0F;  // Data length (lower 4 bits)
    
    // Read data bytes
    for (uint8_t i = 0; i < msg->length && i < 8; i++) {
        msg->data[i] = mcp2515_read(MCP_RXB0SIDH + 5 + i);  // RXB0D0-RXB0D7
    }
    
    // Clear the interrupt flag to indicate message has been read
    mcp2515_bit_modify(MCP_CANINTF, MCP_RX0IF, 0x00);
    
    return 1; // Success
}

// Check if a message is pending
uint8_t can_message_pending(void) {
    uint8_t canintf = mcp2515_read(MCP_CANINTF);
    return (canintf & MCP_RX0IF) ? 1 : 0;
}