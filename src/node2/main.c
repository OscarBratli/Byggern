#include <stdio.h>
#include <stdarg.h>
#include "sam.h"
#include "servo_test.h"
#include "uart.h"
#include "can.h"

#define F_CPU 84000000UL

int main()
{
    SystemInit();

    WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer
    
    // Initialize UART for serial communication
    uart_init(F_CPU, 9600);
    printf("Hello World from Node 2!\n\r");
    
    // Initialize CAN bus with matching bit-timing to Node 1 (125kbps)
    // SAM3X8E CAN runs at MCK/2 = 84MHz/2 = 42MHz
    // 125kbps: BRP=15, PHASE1=6, PHASE2=6, PROPAG=8, SJW=1
    // TQ = (15+1)/42MHz = 0.381us, Bit time = 21 TQ = 8us = 125kbps
    printf("Initializing CAN bus...\n\r");
    can_init((CanInit){
        .brp = 15,      // Baud rate prescaler: (15+1) = 16
        .phase1 = 6,    // Phase 1 segment: 6 TQ
        .phase2 = 6,    // Phase 2 segment: 6 TQ
        .propag = 8,    // Propagation segment: 8 TQ
        .sjw = 1        // Sync jump width: 1 TQ
    }, 0);  // No RX interrupt for now
    printf("CAN initialized at 125kbps!\n\r");
    printf("Waiting for CAN messages from Node 1...\n\r\n");
    
    //servo_test_init(); // Works fine, sets pin high

    while (1)
    {
        // Check for incoming CAN messages
        CanMsg msg;
        if (can_rx(&msg)) {
            printf("Received CAN message:\n\r");
            printf("  ID: 0x%03X\n\r", msg.id);
            printf("  Length: %d\n\r", msg.length);
            printf("  Data: ");
            for (uint8_t i = 0; i < msg.length; i++) {
                printf("0x%02X ", msg.byte[i]);
            }
            printf("\n\r\n");
        }
    }
    
}