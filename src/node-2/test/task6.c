/*
 * task6.c - Node 2 Test Functions
 * 
 * This file contains all the test functions for Node 2 tasks:
 * - Task 1: GPIO Toggle Test (Servo signal pin)
 * - Task 2: UART Communication Test  
 * - Task 3: CAN Communication Test
 * 
 * These functions are called from main.c to keep the main file clean.
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include "sam.h"
#include "uart.h"
#include "can.h"
#include "task6.h"
#include "../pwm.h"
#include "../servo.h"

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
 * Tests CAN initialization and receives messages from Node 1
 * 
 * Uses the exact working configuration from elinemha/TTK4155 repo:
 * - 125 kbps bit rate (perfect match with Node 1)
 * - CAN_BR = 0x00290165 (proven working register value)
 * - Receive-only mode for clean testing
 */
void task3_can_test(void) {
    printf("=== Task 3: CAN Communication Test ===\n");
    printf("Initializing CAN at 125 kbps (EXACT MATCH)...\n");
    
    // WORKING CONFIGURATION from elinemha/TTK4155 repo
    // Use exact CAN_BR register value: 0x00290165
    // This matches their proven working 125 kbps configuration
    // that successfully communicates with MCP2515 Node 1
    
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
    
    // Check CAN error counters
    printf("Initial Error Counters:\n");
    printf("- TEC (Transmit Error): %lu\n", (CAN0->CAN_ECR & CAN_ECR_TEC_Msk) >> CAN_ECR_TEC_Pos);
    printf("- REC (Receive Error): %lu\n\n", (CAN0->CAN_ECR & CAN_ECR_REC_Msk) >> CAN_ECR_REC_Pos);
    
    // Wait a bit before starting
    delay_ms(2000);
    printf("Starting to receive messages from Node 1...\n");
    
    int msg_count = 0;
    int loop_iteration = 0;
    
    // Simple receive-only loop
    while (1) {
        // ONLY RECEIVE - clean and simple
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

/*
 * Checkpoint #8: Joystick Data Decoder for Node 2
 * Structure to hold decoded joystick data
 */
typedef struct {
    uint8_t joy_x;       // Joystick X position (0-100%)
    uint8_t joy_y;       // Joystick Y position (0-100%)
    uint8_t joy_button;  // Joystick button state (0=released, 1=pressed)
    uint8_t slider_x;    // Slider X position (0-255)
    uint8_t slider_y;    // Slider Y position (0-255)
} joystick_data_t;

/*
 * Decode CAN message containing joystick data
 * Returns true if message was successfully decoded as joystick data
 */
bool decode_joystick_message(const CanMsg* msg, joystick_data_t* joy_data) {
    // Check if this is a joystick message (ID 0x100 from Node 1)
    if (msg->id != 0x100 && msg->id != 0x000) {  // Handle both possible ID formats
        return false;  // Not a joystick message
    }
    
    // Check if message has correct length
    if (msg->length != 5) {
        printf("Warning: Joystick message has wrong length (%d, expected 5)\n", msg->length);
        return false;
    }
    
    // Extract joystick data from CAN message
    joy_data->joy_x      = msg->byte[0];  // Joystick X (0-100%)
    joy_data->joy_y      = msg->byte[1];  // Joystick Y (0-100%)
    joy_data->joy_button = msg->byte[2];  // Button state (0/1)
    joy_data->slider_x   = msg->byte[3];  // Slider X (0-255)
    joy_data->slider_y   = msg->byte[4];  // Slider Y (0-255)
    
    return true;  // Successfully decoded
}

/*
 * Display decoded joystick data in a user-friendly format
 */
void display_joystick_data(const joystick_data_t* joy_data) {
    printf("JOY: X=%d%% Y=%d%% Button=%s\n", 
           joy_data->joy_x, joy_data->joy_y,
           joy_data->joy_button ? "PRESSED" : "Released");
    
    printf("SLIDER:   X=%d Y=%d\n", 
           joy_data->slider_x, joy_data->slider_y);
    
    // Visual joystick position indicator (corrected mapping)
    printf("Position: ");
    if (joy_data->joy_y < 30) printf("LEFT ");
    else if (joy_data->joy_y > 70) printf("RIGHT ");
    else printf("CENTER ");
    
    if (joy_data->joy_x < 30) printf("DOWN");
    else if (joy_data->joy_x > 70) printf("UP");
    else printf("MIDDLE");
    
    printf("\n");
}

/*
 * Enhanced CAN test that decodes and displays joystick data
 */
void task3_can_test_with_joystick_decoder(void) {
    printf("=== Task 3: CAN Communication Test (With Joystick Decoder) ===\n");
    printf("Initializing CAN at 125 kbps (EXACT MATCH)...\n");
    
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
    printf("- Mode: Receive-only with joystick decoder\n\n");
    
    printf("Waiting for joystick data from Node 1...\n");
    printf("Move joystick and slider on Node 1 to see decoded values!\n\n");
    
    int msg_count = 0;
    int joystick_msg_count = 0;
    int loop_iteration = 0;
    
    // Enhanced receive loop with joystick decoder
    while (1) {
        CanMsg rx_msg;
        if (can_rx(&rx_msg)) {
            msg_count++;
            
            // Try to decode as joystick message
            joystick_data_t joy_data;
            if (decode_joystick_message(&rx_msg, &joy_data)) {
                joystick_msg_count++;
                printf("=== Joystick Message #%d (Total RX: %d) ===\n", 
                       joystick_msg_count, msg_count);
                
                // Show raw CAN data
                printf("RAW CAN: ID=0x%03X [", rx_msg.id);
                for (int i = 0; i < rx_msg.length; i++) {
                    printf("%02X%s", rx_msg.byte[i], (i < rx_msg.length-1) ? " " : "");
                }
                printf("]\n");
                
                // Show decoded joystick data
                display_joystick_data(&joy_data);
                printf("\n");
            } else {
                // Non-joystick message
                printf("Other CAN: ID=0x%03X [", rx_msg.id);
                for (int i = 0; i < rx_msg.length; i++) {
                    printf("%02X%s", rx_msg.byte[i], (i < rx_msg.length-1) ? " " : "");
                }
                printf("] (Non-joystick)\n");
            }
        }
        
        delay_ms(100);  // Check every 100ms
        loop_iteration++;
        
        // Print statistics every 50 loops (5 seconds)
        if (loop_iteration >= 50) {
            uint32_t tec = (CAN0->CAN_ECR & CAN_ECR_TEC_Msk) >> CAN_ECR_TEC_Pos;
            uint32_t rec = (CAN0->CAN_ECR & CAN_ECR_REC_Msk) >> CAN_ECR_REC_Pos;
            
            printf("=== Statistics ===\n");
            printf("Total CAN messages: %d\n", msg_count);
            printf("Joystick messages: %d\n", joystick_msg_count);
            printf("Error Counters: TEC=%lu, REC=%lu\n", tec, rec);
            if (rec == 0 && msg_count > 0) {
                printf("✓ CAN communication working perfectly!\n");
            }
            printf("\n");
            loop_iteration = 0;
        }
    }
}

/*
 * Task 7: PWM Signal Test
 * Tests the PWM Controller directly to verify signal generation
 * Use oscilloscope to verify 50Hz PWM with variable pulse width
 */
void task7_pwm_test(void) {
    printf("=== Task 7: PWM Controller Test ===\n");
    printf("This test verifies PWM signal generation using the PWM Controller\n");
    printf("Connect oscilloscope to PWM output pin to verify signal\n\n");
    
    // Initialize PWM
    if (!pwm_init()) {
        printf("ERROR: PWM initialization failed!\n");
        return;
    }
    
    printf("PWM initialized successfully!\n");
    pwm_print_status();
    
    printf("Testing different pulse widths...\n");
    printf("Watch oscilloscope for changing pulse widths:\n\n");
    
    uint16_t test_pulses[] = {900, 1200, 1500, 1800, 2100};  // Test pulse widths
    const char* positions[] = {"MIN", "25%", "CENTER", "75%", "MAX"};
    
    int test_count = 0;
    while (1) {
        for (int i = 0; i < 5; i++) {
            printf("Setting PWM: %s position (%d us)\n", positions[i], test_pulses[i]);
            
            if (pwm_set_pulse_width_us(test_pulses[i])) {
                printf("✓ PWM set successfully\n");
            } else {
                printf("✗ PWM set failed\n");
            }
            
            delay_ms(2000);  // Hold each position for 2 seconds
        }
        
        test_count++;
        printf("\n=== Test cycle %d complete ===\n", test_count);
        printf("Pulse width range: 0.9ms - 2.1ms (50Hz period)\n");
        printf("Press Ctrl+C to stop\n\n");
        
        delay_ms(1000);  // Pause between cycles
    }
}

/*
 * Task 7: Servo Control Test  
 * Tests the servo driver with different positions
 */
void task7_servo_test(void) {
    printf("=== Task 7: Servo Control Test ===\n");
    printf("Testing servo control with safety features\n");
    printf("Servo will move through different positions\n\n");
    
    // Initialize servo system
    if (!servo_init()) {
        printf("ERROR: Servo initialization failed!\n");
        return;
    }
    
    printf("Servo initialized successfully!\n");
    servo_print_status();
    
    printf("Testing servo positions...\n");
    printf("IMPORTANT: Verify servo moves smoothly without stuttering\n\n");
    
    uint8_t test_positions[] = {0, 25, 50, 75, 100, 50};  // Test positions
    const char* position_names[] = {"MIN", "25%", "CENTER", "75%", "MAX", "CENTER"};
    
    int test_count = 0;
    while (1) {
        for (int i = 0; i < 6; i++) {
            printf("Moving servo to %s position (%d%%)\n", 
                   position_names[i], test_positions[i]);
            
            if (servo_set_position(test_positions[i])) {
                printf("✓ Servo moved successfully\n");
                servo_print_status();
            } else {
                printf("✗ Servo movement failed\n");
            }
            
            delay_ms(3000);  // Hold each position for 3 seconds
        }
        
        test_count++;
        printf("\n=== Servo test cycle %d complete ===\n", test_count);
        printf("Check for smooth movement without stuttering\n");
        printf("Press Ctrl+C to stop\n\n");
        
        delay_ms(2000);  // Pause between cycles
    }
}

/*
 * Task 7: Servo + Joystick Integration Test
 * Tests servo control using joystick input from CAN
 */
void task7_servo_joystick_test(void) {
    printf("=== Task 7: Servo + Joystick Integration Test ===\n");
    printf("Real-time servo control using joystick from Node 1\n");
    printf("Move joystick on Node 1 to control servo position\n\n");
    
    // Initialize CAN (using working configuration from Task 3)
    uint32_t working_can_br = 0x00290165;
    
    can_init((CanInit){
        .brp = 20, .propag = 2, .phase1 = 7, .phase2 = 6, .sjw = 1, .smp = 0
    }, 0);
    CAN0->CAN_BR = working_can_br;
    
    printf("CAN initialized at 125 kbps\n");
    
    // Initialize servo system
    if (!servo_init()) {
        printf("ERROR: Servo initialization failed!\n");
        return;
    }
    
    printf("Servo initialized successfully!\n");
    printf("Ready for joystick control!\n\n");
    
    printf("Instructions:\n");
    printf("- Move joystick X-axis on Node 1 to control servo\n");
    printf("- Joystick left (0%%) -> Servo minimum position\n");
    printf("- Joystick center (50%%) -> Servo center position\n");  
    printf("- Joystick right (100%%) -> Servo maximum position\n\n");
    
    int msg_count = 0;
    int servo_updates = 0;
    
    while (1) {
        // Check for CAN messages
        CanMsg rx_msg;
        if (can_rx(&rx_msg)) {
            msg_count++;
            
            // Try to decode joystick message
            if (rx_msg.id == 0x100 && rx_msg.length == 5) {
                // Extract joystick data
                uint8_t joy_x = rx_msg.byte[0];
                uint8_t joy_y = rx_msg.byte[1];
                uint8_t joy_button = rx_msg.byte[2];
                
                printf("Joystick: X=%d%% Y=%d%% Button=%s -> ", 
                       joy_x, joy_y, joy_button ? "PRESSED" : "Released");
                
                // Control servo with joystick X position
                if (servo_set_from_joystick_x(joy_x)) {
                    servo_updates++;
                    printf("Servo position updated to %d%%\n", servo_get_position());
                } else {
                    printf("Servo update failed\n");
                }
            } else {
                // Show raw message for debugging
                printf("CAN RX: ID=0x%03X [", rx_msg.id);
                for (int i = 0; i < rx_msg.length; i++) {
                    printf("%02X%s", rx_msg.byte[i], (i < rx_msg.length-1) ? " " : "");
                }
                printf("]\n");
            }
        }
        
        delay_ms(50);  // Check every 50ms for smooth control
        
        // Print periodic status
        static int status_counter = 0;
        if (++status_counter >= 200) {  // Every 10 seconds (200 * 50ms)
            printf("\n=== Status: CAN RX:%d, Servo Updates:%d ===\n", 
                   msg_count, servo_updates);
            servo_print_status();
            status_counter = 0;
        }
    }
}

/*
 * Run all tests in sequence (optional)
 * Uncomment the tests you want to run
 */
void run_all_tests(void) {
    printf("=== Running All Node 2 Tests ===\n\n");
    
    // Uncomment the test you want to run:
    
    // task1_gpio_toggle_test();           // GPIO toggle test
    // task2_uart_test();                  // UART communication test
    // task3_can_test();                   // Basic CAN communication test
    task3_can_test_with_joystick_decoder(); // CAN with joystick decoder ✓
    
    printf("All tests completed!\n");
}
