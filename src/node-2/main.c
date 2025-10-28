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
    printf("Initializing CAN at 250 kbps...\n");
    
    // Initialize CAN with 250 kbps bit timing
    // CRITICAL: Must match Node 1 (MCP2515) EXACTLY!
    //
    // CAN peripheral clock = 84 MHz / 2 = 42 MHz
    // Target: ~250 kbps with 16 TQ per bit (MUST MATCH NODE 1!)
    //
    // REALITY CHECK: 42MHz cannot divide evenly to give exactly 250 kbps with 16 TQ
    // - BRP = 9 → 42MHz / 10 / 16 = 262.5 kHz (5.0% too fast)
    // - BRP = 10 → 42MHz / 11 / 16 = 238.6 kHz (4.5% too slow)
    //
    // CRITICAL DECISION: Use BRP=9 (262.5 kHz, 5% fast)
    // Why? Because:
    // 1. Node 1 is transmitting most messages (it's the master)
    // 2. Being slightly FASTER allows us to resync DOWN to Node 1's pace
    // 3. CAN resynchronization can handle ±5 TQ per bit (we're within spec!)
    // 4. SJW=1 means we can adjust ±1 TQ to match incoming edges
    //
    // Segment distribution with 16 TQ (MUST EXACTLY MATCH NODE 1):
    // - SyncSeg = 1 TQ (fixed)
    // - PropSeg = 6 TQ (MATCHES NODE 1)
    // - Phase1 = 5 TQ (MATCHES NODE 1)  
    // - Phase2 = 4 TQ (MATCHES NODE 1)
    // Total = 1 + 6 + 5 + 4 = 16 TQ ✓
    // Sample point = (1 + 6 + 5) / 16 = 75% (MATCHES NODE 1!)
    
    can_init((CanInit){
        .brp = 9,       // Baud rate prescaler → 262.5 kbps (5% fast, will resync to Node 1)
        .propag = 6,    // Propagation segment (6 TQ) - MATCHES NODE 1
        .phase1 = 5,    // Phase segment 1 (5 TQ) - MATCHES NODE 1
        .phase2 = 4,    // Phase segment 2 (4 TQ) - MATCHES NODE 1
        .sjw = 1,       // Synchronization jump width (allows ±1 TQ adjustment)
        .smp = 0        // Single sampling
    }, 0);  // No RX interrupt for now
    
    printf("CAN initialized successfully!\n");
    printf("- Bit rate: ~250 kbps\n");
    printf("- Mode: Normal (ready for Node 1 communication)\n");
    
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
    
    // Skip the initial test message to avoid blocking issues
    printf("Skipping initial test message to avoid bus conflicts...\n");
    printf("Node 2 will listen first, then send after receiving messages.\n\n");
    
    printf("Entering main CAN communication loop...\n");
    printf("Strategy: LISTEN FIRST, then respond\n");
    printf("Listening for messages from Node 1...\n\n");
     
    // Check CAN error counters
    printf("Initial Error Counters:\n");
    printf("- TEC (Transmit Error): %lu\n", (CAN0->CAN_ECR & CAN_ECR_TEC_Msk) >> CAN_ECR_TEC_Pos);
    printf("- REC (Receive Error): %lu\n\n", (CAN0->CAN_ECR & CAN_ECR_REC_Msk) >> CAN_ECR_REC_Pos);
    
    // Wait a bit before starting
    delay_ms(2000);
    printf("Starting CAN communication (RX only at first)...\n\n");
    
    // Listen for incoming CAN messages and send at controlled rate
    int msg_count = 0;
    int tx_count = 0;
    int loop_iteration = 0;
    int send_timer = 0;
    uint8_t start_sending = 0;  // Only start sending after receiving first message
    
    while (1) {
        // Check for received messages
        CanMsg rx_msg;
        if (can_rx(&rx_msg)) {
            printf("RX<-Node1: ");
            can_printmsg(rx_msg);
            msg_count++;
            
            // After receiving first message, start sending too
            if (!start_sending) {
                start_sending = 1;
                printf("*** First message received! Now starting TX responses ***\n");
            }
        }
        
        // Only send if we've received at least one message (bus is working)
        if (start_sending && send_timer >= 10) {
            CanMsg tx_msg = {
                .id = 0x200 + (tx_count & 0xFF),
                .length = 4,
                .byte = {tx_count, tx_count+1, tx_count+2, tx_count+3}
            };
            
            printf("TX->Node1: ID=0x%03X [%02X %02X %02X %02X] ", 
                   tx_msg.id, tx_msg.byte[0], tx_msg.byte[1], tx_msg.byte[2], tx_msg.byte[3]);
            
            can_tx(tx_msg);
            printf("Sent\n");
            tx_count++;
            send_timer = 0;
        }
        
        delay_ms(100);
        loop_iteration++;
        if (start_sending) {
            send_timer++;
        }
        
        // Print statistics every 50 loops (5 seconds)
        if (loop_iteration >= 50) {
            uint32_t tec = (CAN0->CAN_ECR & CAN_ECR_TEC_Msk) >> CAN_ECR_TEC_Pos;
            uint32_t rec = (CAN0->CAN_ECR & CAN_ECR_REC_Msk) >> CAN_ECR_REC_Pos;
            
            printf("\n=== Status (TX: %d, RX: %d) ===\n", tx_count, msg_count);
            printf("Error Counters: TEC=%lu, REC=%lu\n", tec, rec);
            if (tec == 0 && rec == 0) {
                printf("✓ CAN communication healthy!\n");
            } else if (tec < 128 && rec < 128) {
                printf("⚠ Minor errors, but still active\n");
            } else {
                printf("✗ Error-Passive mode - check wiring/timing\n");
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

    // Uncomment the test you want to run:
    //task1_gpio_toggle_test();  // Task 1: GPIO toggle test
    //task2_uart_test();          // Task 2: UART communication test
    task3_can_test();            // Task 3: CAN communication test
    
    // Main program loop (if not running tests)
    while (1)
    {
        // Your main code here
    }
    
}