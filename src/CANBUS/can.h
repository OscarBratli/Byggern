#ifndef CAN_H
#define CAN_H

#include <stdint.h>

// CAN message structure
typedef struct {
    uint16_t id;           // CAN message ID (11-bit standard ID)
    uint8_t length;        // Data length (0-8 bytes)
    uint8_t data[8];       // Message data
} can_message_t;

// CAN driver function declarations
void can_init(void);                                    // Initialize CAN controller in loopback mode
uint8_t can_send_message(can_message_t* msg);          // Send a CAN message
uint8_t can_receive_message(can_message_t* msg);       // Receive a CAN message
uint8_t can_message_pending(void);                     // Check if message is pending

#endif