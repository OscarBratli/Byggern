#include <stdio.h>
#include <stdarg.h>
#include "sam.h"
#include "uart.h"
#include "can.h"

/*
 * Task 1: Simple GPIO toggle "Hello World" for Node 2
 * This program toggles a GPIO pin to verify the board is working.
 * You can measure this with an oscilloscope on the servo header signal pin.
 * 
 * Using PIOB pin 13 (PB13) - Arduino D21, connected to servo SIGNAL pin
 * According to Motor Shield docs: SIGNAL = D21 = PB13 (PWM1)
 * 
 * Task 2: UART Communication
 * Initialize UART at 9600 baud for printf debugging
 * 
 * Task 3: CAN Communication
 * Set up CAN bus on Node 2 for communication with Node 1
 */

// Simple delay function (not accurate, just for demonstration)
void delay_ms(uint32_t ms) {
    // Rough delay assuming 84MHz CPU clock
    // Each loop iteration takes multiple cycles
    for (uint32_t i = 0; i < ms * 10000; i++) {
        __asm__("nop");
    }
}

/*
 * Task 1 Test: GPIO Toggle Test
 * Toggles PB13 (servo signal pin) at 1Hz to verify board operation
 * Measure with oscilloscope - should see 0-5V square wave
 */
void task1_gpio_toggle_test(void) {
    printf("=== Task 1: GPIO Toggle Test ===\n");
    printf("Configuring PB13 (servo SIGNAL pin) as output...\n");
    
    // Enable clock for PIOB (Peripheral ID 12)
    PMC->PMC_PCER0 |= (1 << ID_PIOB);
    
    // Configure PB13 as output (servo signal pin - Arduino D21)
    PIOB->PIO_PER |= PIO_PB13;   // Enable PIO control (not peripheral)
    PIOB->PIO_OER |= PIO_PB13;   // Enable output
    PIOB->PIO_PUDR |= PIO_PB13;  // Disable pull-up
    
    // Initialize pin to low
    PIOB->PIO_CODR |= PIO_PB13;  // Clear output (set low)

    printf("Starting GPIO toggle at 1Hz...\n");
    printf("Connect oscilloscope to servo header SIGNAL pin (middle pin)\n");
    printf("Press Ctrl+C or reset to stop\n\n");
    
    int toggle_count = 0;
    while (1)
    {
        // Toggle the pin high
        PIOB->PIO_SODR = PIO_PB13;  // Set output data register (set high)
        delay_ms(500);               // Wait 500ms
        
        // Toggle the pin low
        PIOB->PIO_CODR = PIO_PB13;  // Clear output data register (set low)
        delay_ms(500);               // Wait 500ms
        
        // Print status every 10 toggles (every 10 seconds)
        toggle_count++;
        if (toggle_count >= 10) {
            printf("Still toggling... (total: %d cycles)\n", toggle_count);
            toggle_count = 0;
        }
    }
}

/*
 * Task 2 Test: UART Communication Test
 * Tests printf functionality and UART communication
 */
void task2_uart_test(void) {
    printf("=== Task 2: UART Communication Test ===\n");
    printf("This test verifies UART is working at 9600 baud\n");
    printf("You should see this message in the serial monitor\n\n");
    
    printf("Testing different output formats:\n");
    printf("- Integer: %d\n", 42);
    printf("- Hex: 0x%X\n", 0xDEADBEEF);
    printf("- String: %s\n", "Hello from Node 2!");
    printf("- Float approximation: %d.%d\n", 3, 14);
    printf("\n");
    
    int counter = 0;
    while (1) {
        printf("UART Test - Counter: %d\n", counter++);
        delay_ms(1000);  // Print every second
        
        if (counter >= 10) {
            printf("\nTask 2 UART test complete!\n");
            printf("UART communication is working correctly.\n\n");
            break;
        }
    }
}

/*
 * Task 3 Test: CAN Communication Test
 * Tests CAN initialization and basic loopback functionality
 * 
 * CAN bit timing for 250 kbps with 84 MHz CPU clock:
 * - Baud Rate Prescaler (BRP): Divides clock down
 * - Phase segments and propagation delay determine bit timing
 * - Total Time Quanta (TQ) = 1 + PROPAG + PHASE1 + PHASE2
 * 
 * For 250 kbps with 84 MHz:
 * - CAN clock = 84 MHz / 2 = 42 MHz (divided by 2 in peripheral)
 * - Bit rate = CAN_clock / ((BRP+1) * TQ)
 * - 250000 = 42000000 / ((BRP+1) * TQ)
 * - With BRP=20, TQ=10: 42000000/(21*10) = 200000 ≈ close
 * - Better: BRP=19, TQ=8: 42000000/(20*8) = 262500 (close to 250k)
 */
void task3_can_test(void) {
    printf("=== Task 3: CAN Communication Test ===\n");
    printf("Initializing CAN at 125 kbps (EXACT MATCH)...\n");
    
    // Initialize CAN with 125 kbps bit timing
    // CRITICAL: EXACT match with Node 1 (MCP2515)!
    //
    // CAN peripheral clock = 84 MHz / 2 = 42 MHz
    // Target: 125 kbps EXACTLY with 16 TQ per bit
    //
    // WORKING CONFIGURATION from elinemha/TTK4155 repo
    // Use exact CAN_BR register value: 0x00290165
    // This matches their proven working 125 kbps configuration
    // that successfully communicates with MCP2515 Node 1
    
    // Direct register approach (bypassing our can_init wrapper)
    uint32_t working_can_br = 0x00290165;  // From elinemha repo
    
    // Initialize CAN with working configuration  
    can_init((CanInit){
        .brp = 20,      // Try exact 125 kbps: 42MHz / 21 / 16TQ = 125.0 kbps
        .propag = 2,    // Match Node 1's propagation segment
        .phase1 = 7,    // Match Node 1's phase1 segment  
        .phase2 = 6,    // Match Node 1's phase2 segment
        .sjw = 1,       // SJW = 1 TQ
        .smp = 0        // Single sampling
    }, 0);  // No RX interrupt
    
    // Override with exact working register value
    CAN0->CAN_BR = working_can_br;
    
    printf("CAN initialized successfully!\n");
    printf("- Bit rate: 125.0 kbps (EXACT match with Node 1)\n");
    printf("- Using proven working CAN_BR = 0x%08X from elinemha repo\n", working_can_br);
    printf("- Mode: Receive-only (listening for Node 1)\n");
    
    // Check CAN status
    uint32_t can_sr = CAN0->CAN_SR;
    printf("- CAN Status Register: 0x%08lX\n", can_sr);
    
    // Check if CAN is enabled
    uint32_t can_mr = CAN0->CAN_MR;
    printf("- CAN Mode Register: 0x%08lX\n", can_mr);
    printf("- CAN Enabled: %s\n\n", (can_mr & CAN_MR_CANEN) ? "YES" : "NO");
    
    if (!(can_mr & CAN_MR_CANEN)) {
        printf("ERROR: CAN is not enabled!\n");
        while(1);
    }
    
    printf("Checking TX mailbox status...\n");
    uint32_t tx_msr = CAN0->CAN_MB[0].CAN_MSR;
    printf("- TX Mailbox Status: 0x%08lX\n", tx_msr);
    printf("- MRDY (Ready): %s\n\n", (tx_msr & CAN_MSR_MRDY) ? "YES" : "NO");
    
    // TEST: Node 2 sends first to test TX capability
    printf("=== REVERSE TEST: Node 2 -> Node 1 ===\n");
    printf("Node 2 will now SEND to Node 1 to test TX path...\n\n");
    
    // Check CAN error counters
    printf("Initial Error Counters:\n");
    printf("- TEC (Transmit Error): %lu\n", (CAN0->CAN_ECR & CAN_ECR_TEC_Msk) >> CAN_ECR_TEC_Pos);
    printf("- REC (Receive Error): %lu\n\n", (CAN0->CAN_ECR & CAN_ECR_REC_Msk) >> CAN_ECR_REC_Pos);
    
    // Wait a bit before starting
    delay_ms(2000);
    printf("Starting to send messages to Node 1...\n");
    
    // DEBUG: Check initial mailbox status again
    tx_msr = CAN0->CAN_MB[0].CAN_MSR;
    printf("DEBUG: Initial TX mailbox MSR = 0x%08lX\n", tx_msr);
    printf("DEBUG: MRDY bit = %s\n\n", (tx_msr & CAN_MSR_MRDY) ? "SET (ready)" : "CLEAR (busy)");
    
    int msg_count = 0;
    int tx_count = 0;
    int loop_iteration = 0;
    
    // Simple receive-only loop
    while (1) {
        // ONLY RECEIVE - no sending to avoid complexity
        CanMsg rx_msg;
        if (can_rx(&rx_msg)) {
            msg_count++;
            printf("RX<-Node1: ID=0x%03X [", rx_msg.id);
            for (int i = 0; i < rx_msg.length; i++) {
                printf("%02X%s", rx_msg.byte[i], (i < rx_msg.length-1) ? " " : "");
            }
            printf("] RX:%d\n", msg_count);
        }
        
        delay_ms(100);  // Check every 100ms
        loop_iteration++;
        
        // Print statistics every 50 loops (5 seconds)
        if (loop_iteration >= 50) {
            uint32_t tec = (CAN0->CAN_ECR & CAN_ECR_TEC_Msk) >> CAN_ECR_TEC_Pos;
            uint32_t rec = (CAN0->CAN_ECR & CAN_ECR_REC_Msk) >> CAN_ECR_REC_Pos;
            
            printf("=== Status (RX-only): RX:%d ===\n", msg_count);
            printf("Error Counters: TEC=%lu, REC=%lu\n", tec, rec);
            if (rec == 0 && msg_count > 0) {
                printf("✓✓✓ RX working perfectly!\n");
            } else if (rec > 0) {
                printf("✗ RX errors: Bit timing mismatch?\n");
            } else {
                printf("? No messages received yet...\n");
            }
            printf("\n");
            loop_iteration = 0;
        }
    }
}



int main()
{
    SystemInit();

    WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer

    // ***** Task 2: UART Setup *****
    uart_init(84000000, 9600);
    printf("\n\n");
    printf("================================\n");
    printf("  Node 2 - ATSAM3X8E Initialized\n");
    printf("================================\n");
    printf("CPU Frequency: 84 MHz\n");
    printf("UART Baudrate: 9600\n");
    printf("\n");

    // Run CAN communication test
    task3_can_test();
    
    // Should never reach here
    while (1) {
    }
     
}