/*
 * task8.c - Motor Control and Encoder Tests
 */

#include <stdio.h>
#include "sam.h"
#include "../uart.h"
#include "../encoder.h"
#include "../motor.h"
#include "../servo.h"
#include "../can.h"
#include "../pi_controller.h"
#include "../time.h"
#include "task8.h"

static void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 10000; i++) {
        __asm__("nop");
    }
}

/**
 * Test encoder reading
 * 
 * This test initializes the encoder and continuously displays
 * the encoder position. Manually rotate the motor shaft to verify
 * that the encoder is working correctly.
 * 
 * Expected behavior:
 * - Turning motor clockwise increases count
 * - Turning motor counter-clockwise decreases count
 * - Position counter should be stable when motor is stationary
 * - One full revolution should give approximately ±360 counts
 */
void task8_encoder_test(void) {
    printf("\n=== Task 8: Encoder Test ===\n");
    
    // Initialize encoder
    if (!encoder_init()) {
        printf("ERROR: Failed to initialize encoder!\n");
        return;
    }
    
    printf("Encoder initialized successfully!\n");
    printf("Starting continuous reading...\n\n");
    
    // Reset position to 0
    encoder_reset();
    
    int16_t last_position = 0;
    uint32_t update_counter = 0;
    
    while (1) {
        int16_t current_position = encoder_read();
        
        // Only print when position changes or every second
        if (current_position != last_position || (update_counter % 100 == 0)) {
            float revolutions = encoder_get_revolutions();
            int16_t delta = current_position - last_position;
            
            // Determine direction based on change
            const char* direction = "STOP";
            if (delta > 0) {
                direction = "LEFT";
            } else if (delta < 0) {
                direction = "RIGHT";
            }
            
            printf("Pos: %6d | Rev: %+7.3f | Delta: %+5d | Dir: %s\n", 
                   current_position,
                   revolutions,
                   delta,
                   direction);
            
            last_position = current_position;
        }
        
        update_counter++;
        delay_ms(10);  // 100Hz update rate
    }
}

/**
 * Test motor control with joystick
 * 
 * This combines motor control with servo control for full game functionality.
 * - Joystick X-axis controls motor position/speed
 * - Joystick Y-axis controls servo angle (already implemented in Task 7)
 * - Encoder provides position feedback
 * 
 * This is open-loop control - just mapping joystick directly to motor speed.
 * A PI controller (Task 8 Step 4) will improve this later.
 */
void task8_joystick_motor_control(void) {
    printf("\n=== Task 8: Joystick Motor Control (Open-Loop) ===\n");
    printf("Control motor and servo with joystick via CAN\n");
    printf("X-axis: Motor position (left/right)\n");
    printf("Y-axis: Servo angle (already implemented)\n");
    printf("Note: This is direct speed control without feedback\n");
    printf("Game is playable but hard to control!\n\n");
    
    // Initialize all subsystems
    if (!encoder_init()) {
        printf("ERROR: Failed to initialize encoder!\n");
        return;
    }
    
    if (!motor_init()) {
        printf("ERROR: Failed to initialize motor!\n");
        return;
    }
    
    // Start with motor stopped
    motor_set_signed(0);
    printf("Motor initialized and set to STOP (0%% PWM duty)\n\n");
    
    // Initialize servo (from Task 7)
    if (!servo_init()) {
        printf("ERROR: Failed to initialize servo!\n");
        return;
    }
    
    // Initialize CAN (use proven working config from Task 7)
    uint32_t working_can_br = 0x00290165;
    can_init((CanInit){
        .brp = 20, .propag = 2, .phase1 = 7, .phase2 = 6, .sjw = 1, .smp = 0
    }, 0);
    CAN0->CAN_BR = working_can_br;
    
    printf("CAN initialized - waiting for joystick data...\n\n");
    
    // Start with motor stopped
    motor_set_signed(0);
    printf("Motor initialized to STOP\n\n");
    
    CanMsg msg;
    uint8_t last_x = 255;  // Initialize to invalid value to force first update
    uint8_t last_y = 255;  // Initialize to invalid value to force first update
    
    while (1) {
        // Check for CAN messages
        if (can_rx(&msg)) {
            // Joystick data (ID = 0x00): [X, Y, ?, Button, Slider]
            if (msg.id == 0x00 && msg.length >= 2) {
                uint8_t joy_x = msg.byte[0];  // X-axis for motor (0-100%)
                uint8_t joy_y = msg.byte[1];  // Y-axis for servo (0-100%)
                
                // ========== MOTOR CONTROL (X-axis) ==========
                // Map joystick X to motor speed: -100 to +100
                // X=0 (left) → -100 (full left)
                // X=50 (center) → 0 (stop)
                // X=100 (right) → +100 (full right)
                
                int8_t motor_speed = (int8_t)((joy_x - 50) * 2);  // Map 0-100 to -100 to +100
                
                // Apply deadzone near center (±5% deadzone = ±10 speed units)
                if (motor_speed > -10 && motor_speed < 10) {
                    motor_speed = 0;
                }
                
                // Always update motor (no change threshold - we want instant response)
                if (joy_x != last_x) {
                    motor_set_signed(motor_speed);
                    last_x = joy_x;
                    
                    // Debug output
                    printf("Joy X=%3d%% → Motor Speed=%+4d%%\n", joy_x, motor_speed);
                }
                
                // ========== SERVO CONTROL (Y-axis) ==========
                // Invert Y-axis: Y=0 (top) → 100% (max angle), Y=100 (bottom) → 0% (min angle)
                uint8_t servo_percent = 100 - joy_y;
                
                // Only update if changed significantly
                if (abs(joy_y - last_y) > 1) {
                    servo_set_position(servo_percent);
                    last_y = joy_y;
                }
                
                // Optional: Print encoder position for debugging
                // int16_t pos = encoder_read();
                // printf("Encoder: %d | Motor: %d%% | Servo: %d%%\n", pos, motor_speed, servo_percent);
            }
        }
        
        // No delay - poll as fast as possible for instant response
    }
}

/**
 * PI Position Controller with Joystick (Task 8 Step 4)
 * 
 * This implements closed-loop position control using encoder feedback.
 * - Joystick X-axis sets TARGET position (not speed!)
 * - PI controller calculates motor speed to reach target
 * - Much easier to control than open-loop!
 */
void task8_pi_position_control(void) {
    printf("\n=== Task 8: PI Position Controller (Closed-Loop) ===\n");
    printf("Joystick X-axis: Target position (left/right)\n");
    printf("Joystick Y-axis: Servo angle\n");
    printf("PI controller automatically adjusts motor speed\n");
    printf("Game should be much easier to play!\n\n");
    
    // Initialize encoder
    if (!encoder_init()) {
        printf("ERROR: Failed to initialize encoder!\n");
        return;
    }
    
    // Initialize motor
    if (!motor_init()) {
        printf("ERROR: Failed to initialize motor!\n");
        return;
    }
    
    // Start with motor stopped
    motor_set_signed(0);
    printf("Motor initialized and set to STOP\n");
    
    // Initialize servo
    if (!servo_init()) {
        printf("ERROR: Failed to initialize servo!\n");
        return;
    }
    
    // Initialize CAN
    uint32_t working_can_br = 0x00290165;
    can_init((CanInit){
        .brp = 20, .propag = 2, .phase1 = 7, .phase2 = 6, .sjw = 1, .smp = 0
    }, 0);
    CAN0->CAN_BR = working_can_br;
    printf("CAN initialized\n\n");
    
    // Initialize PI controller
    // Tuned gains - increased from conservative starting point
    // System is stable, so we can increase responsiveness
    pi_controller_t pi_ctrl;
    pi_controller_init(&pi_ctrl, 
                       8.0f,    // Kp - Proportional gain (increased for faster response)
                       2.0f,    // Ki - Integral gain (helps reach target)
                       1.5f,    // Kd - Derivative term for damping overshoot
                       2000.0f); // integral_max - prevent windup
    
    // Define the position range
    // From working repo analysis: henrikhestnes/TTK4155-Byggern
    // - Encoder returns SIGNED values (negative at left, positive at right)
    // - They scale to SLIDER_MAX=100 (0-100 range)
    // - Formula: SLIDER_MAX * value / (MAX_ENCODER_VALUE - MIN_ENCODER_VALUE)
    // - But encoder is signed, so we treat MIN as the negative left limit
    // Our encoder: ±9000 raw counts → scale to 0-100 for position control
    const int16_t ENCODER_MAX_RAW = 8800;  // From working repo (their MAX_ENCODER_VALUE)
    const int16_t ENCODER_MIN_RAW = -8800; // Signed range
    const int16_t POSITION_MAX = 100;      // Match their SLIDER_MAX
    const int16_t POSITION_MIN = 0;
    
    // ========== CALIBRATION FUNCTION ==========
    // Simple time-based calibration to find mechanical limits
    // This runs automatically on every boot
    printf("\n=== Starting Calibration Sequence ===\n");
    printf("Please wait while the system calibrates...\n");
    
    int16_t edge_left_raw = 0;   // Will store left endpoint
    int16_t edge_right_raw = 0;  // Will store right endpoint
    const int16_t EDGE_MARGIN = 200;  // Safety margin
    
    // Phase 1: Drive in one direction for 5 seconds at 30% speed
    printf("\nPhase 1: Driving at -30%% for 5 seconds...\n");
    motor_set_signed(-30);  // 30% speed in negative direction
    time_spinFor(seconds(5));  // Wait 5 seconds
    
    // Record first endpoint
    edge_left_raw = encoder_read();
    printf("First endpoint: encoder = %d\n", edge_left_raw);
    
    motor_set_signed(0);  // Stop briefly
    time_spinFor(msecs(500));
    
    // Phase 2: Drive in opposite direction for 5 seconds at 30% speed
    printf("\nPhase 2: Driving at +30%% for 5 seconds...\n");
    motor_set_signed(30);  // 30% speed in positive direction
    time_spinFor(seconds(5));  // Wait 5 seconds
    
    // Record second endpoint
    edge_right_raw = encoder_read();
    printf("Second endpoint: encoder = %d\n", edge_right_raw);
    
    motor_set_signed(0);  // Stop
    time_spinFor(msecs(500));
    
    // Determine which is min and which is max (encoder is signed!)
    int16_t edge_min_raw, edge_max_raw;
    if (edge_left_raw < edge_right_raw) {
        edge_min_raw = edge_left_raw;
        edge_max_raw = edge_right_raw;
        printf("Direction detected: -30%% goes to MIN, +30%% goes to MAX\n");
    } else {
        edge_min_raw = edge_right_raw;
        edge_max_raw = edge_left_raw;
        printf("Direction detected: -30%% goes to MAX, +30%% goes to MIN\n");
    }
    
    printf("Detected range: MIN=%d, MAX=%d\n", edge_min_raw, edge_max_raw);
    
    // Apply safety margins
    edge_min_raw += EDGE_MARGIN;
    edge_max_raw -= EDGE_MARGIN;
    
    // Phase 3: Return to center
    printf("\nPhase 3: Returning to center...\n");
    int16_t center_raw = (edge_min_raw + edge_max_raw) / 2;
    printf("Center position: %d\n", center_raw);
    
    // Use proportional control to reach center smoothly
    bool centered = false;
    for (int i = 0; i < 5000; i++) {  // Max 5 seconds
        int16_t current = encoder_read();
        int16_t error = center_raw - current;
        
        // Check if we're close enough to center
        if (abs(error) < 50) {
            printf("Reached center at encoder = %d\n", current);
            centered = true;
            break;
        }
        
        // Proportional control: speed proportional to error
        int8_t motor_cmd = (int8_t)(error / 15);  // Gentle approach
        if (motor_cmd > 40) motor_cmd = 40;
        if (motor_cmd < -40) motor_cmd = -40;
        motor_set_signed(motor_cmd);
        
        time_spinFor(msecs(1));
    }
    
    motor_set_signed(0);  // Ensure motor is stopped
    
    if (!centered) {
        printf("Warning: Could not reach exact center, using current position\n");
    }
    
    time_spinFor(msecs(500));
    
    printf("\n=== Calibration Complete ===\n");
    printf("Safe range: %d to %d (with %d count margins)\n", 
           edge_min_raw, edge_max_raw, EDGE_MARGIN);
    printf("Total travel: %d counts\n", edge_max_raw - edge_min_raw);
    printf("\n=== Ready for Game Play ===\n");
    printf("Waiting for joystick input...\n\n");
    
    // Update the scaling constants based on actual measured range
    const int16_t ENCODER_MAX_RAW_ACTUAL = edge_max_raw;
    const int16_t ENCODER_MIN_RAW_ACTUAL = edge_min_raw;
    
    CanMsg msg;
    uint8_t last_y = 255;
    uint8_t last_joy_x = 50;  // Track last joystick X value for debug
    int8_t joy_motor_cmd = 0;  // Motor command from joystick (for open loop testing)
    uint32_t loop_counter = 0;
    bool joystick_received = false;  // Flag to ensure we got joystick data before running motor
    uint32_t can_msg_count = 0;  // Count CAN messages received
    uint32_t last_can_count = 0;
    
    printf("Starting control loop...\n");
    printf("Waiting for joystick data before enabling motor...\n");
    printf("DEBUG MODE: Will show encoder changes and joystick values\n\n");
    
    int16_t last_encoder_check = 0;
    uint32_t encoder_check_counter = 0;
    
    while (1) {
        // Read current position (raw encoder counts, signed: negative=left, positive=right)
        int16_t encoder_raw = encoder_read();
        
        // Enforce edge limits - stop motor if we're at the mechanical limits
        bool at_left_edge = (encoder_raw <= edge_min_raw);
        bool at_right_edge = (encoder_raw >= edge_max_raw);
        
        // Scale encoder reading to 0-100 range using ACTUAL measured limits
        // Formula: current_position = (encoder_raw - ENCODER_MIN_RAW_ACTUAL) * POSITION_MAX / (ENCODER_MAX_RAW_ACTUAL - ENCODER_MIN_RAW_ACTUAL)
        int16_t current_position = (int16_t)(((int32_t)(encoder_raw - ENCODER_MIN_RAW_ACTUAL) * POSITION_MAX) / (ENCODER_MAX_RAW_ACTUAL - ENCODER_MIN_RAW_ACTUAL));
        
        // Clamp to valid range (0-100)
        if (current_position > POSITION_MAX) current_position = POSITION_MAX;
        if (current_position < POSITION_MIN) current_position = POSITION_MIN;
        
        // Check for CAN messages (joystick data)
        if (can_rx(&msg)) {
            can_msg_count++;  // Count messages
            
            if (msg.id == 0x00 && msg.length >= 2) {
                uint8_t joy_x = msg.byte[0];  // X-axis for target position (0-100%)
                uint8_t joy_y = msg.byte[1];  // Y-axis for servo (0-100%)
                
                last_joy_x = joy_x;  // Store for debug output
                
                // Mark that we've received joystick data
                if (!joystick_received) {
                    joystick_received = true;
                    printf("Joystick data received! Motor control enabled.\n");
                    printf("Initial joystick X = %d%%\n", joy_x);
                    printf("Make sure to move joystick to see if values change!\n\n");
                }
                
                // ========== POSITION CONTROL (X-axis) ==========
                // HYBRID CONTROL: Joystick controls target position, PI controller smooths motion
                
                // Map joystick to target position (0 to POSITION_MAX = 0 to 100)
                // Joystick X: 0-100% maps to position 0-100
                // This matches the working repo's slider range (0-100)
                int16_t target_position = ((int32_t)joy_x * POSITION_MAX) / 100;
                
                // Apply slew rate limiting to prevent huge target jumps
                // Allow faster target changes now that controller is stable
                // Max ±10 per update (at 10kHz = 100/ms)
                static int16_t last_target = 50;  // Start at center
                int16_t target_delta = target_position - last_target;
                if (target_delta > 10) target_delta = 10;
                if (target_delta < -10) target_delta = -10;
                target_position = last_target + target_delta;
                last_target = target_position;
                
                // Update PI controller target
                pi_controller_set_target(&pi_ctrl, target_position);
                
                // ========== SERVO CONTROL (Y-axis) ==========
                uint8_t servo_percent = 100 - joy_y;  // Inverted
                if (abs(joy_y - last_y) > 1) {
                    servo_set_position(servo_percent);
                    last_y = joy_y;
                }
            }
        }
        
        // Only run motor control if we've received joystick data
        int8_t motor_cmd = 0;
        if (joystick_received) {
            // Use PI controller for smooth position control
            int8_t pi_output = pi_controller_update(&pi_ctrl, current_position);
            
            // Limit motor speed to prevent mechanical damage
            // Increased from ±30% to ±50% for better response
            if (pi_output > 50) motor_cmd = 50;
            else if (pi_output < -50) motor_cmd = -50;
            else motor_cmd = pi_output;
            
            // EDGE PROTECTION: Stop motor if we're at limits
            // Prevent driving further into mechanical stops
            if (at_left_edge && motor_cmd < 0) {
                motor_cmd = 0;  // Don't drive further left
                printf("WARNING: At LEFT edge, blocking leftward motion\n");
            }
            if (at_right_edge && motor_cmd > 0) {
                motor_cmd = 0;  // Don't drive further right
                printf("WARNING: At RIGHT edge, blocking rightward motion\n");
            }
            
            // Send command to motor
            motor_set_signed(motor_cmd);
            
            // Debug: Print compact status line
            static uint16_t debug_counter = 0;
            debug_counter++;
            if (debug_counter >= 1000) {  // Every 1000 loops (100ms at 10kHz)
                int16_t error = pi_ctrl.target - current_position;  // Both in 0-100 range now
                printf("JoyX=%3d%% | MotorCmd=%+4d%% | EncRaw=%+5d | EncScaled=%3d | Target=%3d | Error=%+4d\n", 
                       last_joy_x,
                       motor_cmd, 
                       encoder_raw,
                       current_position,  // Now 0-100 range
                       pi_ctrl.target,    // Also 0-100 range
                       error);
                debug_counter = 0;
            }
        } else {
            // Keep motor stopped until we get joystick data
            motor_set_signed(0);
        }
        
        // Fast control loop timing (0.1ms = 10kHz control rate)
        // Required for aggressive PID gains Kp=6, Ki=8, Kd=0.8
        time_spinFor(usecs(100));  // 100 microseconds = 0.1ms
    }
}
