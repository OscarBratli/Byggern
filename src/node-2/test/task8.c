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
#include "../time.h"
#include "../solenoid.h"
#include "../ir_sensor.h"
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
 */
void task8_encoder_test(void) {
    if (!encoder_init()) {
        printf("ERROR: Failed to initialize encoder!\n");
        return;
    }
    
    encoder_reset();
    
    int16_t last_position = 0;
    uint32_t update_counter = 0;
    
    while (1) {
        int16_t current_position = encoder_read();
        
        if (current_position != last_position || (update_counter % 100 == 0)) {
            float revolutions = encoder_get_revolutions();
            int16_t delta = current_position - last_position;
            
            const char* direction = "STOP";
            if (delta > 0) {
                direction = "LEFT";
            } else if (delta < 0) {
                direction = "RIGHT";
            }
            
            printf("Pos: %6d | Rev: %+7.3f | Delta: %+5d | Dir: %s\n", 
                   current_position, revolutions, delta, direction);
            
            last_position = current_position;
        }
        
        update_counter++;
        delay_ms(10);
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
    
    // Initialize solenoid
    solenoid_init();
    printf("Solenoid initialized\n");
    
    // Initialize CAN
    uint32_t working_can_br = 0x00290165;
    can_init((CanInit){
        .brp = 20, .propag = 2, .phase1 = 7, .phase2 = 6, .sjw = 1, .smp = 0
    }, 0);
    CAN0->CAN_BR = working_can_br;
    
    // Start with motor stopped
    motor_set_signed(0);
    
    CanMsg msg;
    uint8_t last_x = 255;
    uint8_t last_y = 255;
    uint8_t last_button = 0;
    
    while (1) {
        // Check for CAN messages
        if (can_rx(&msg)) {
            // Joystick data (ID = 0x00): [X, Y, JoyBtn, SliderX, SliderY]
            if (msg.id == 0x00 && msg.length >= 3) {
                uint8_t joy_x = msg.byte[0];      // X-axis for motor (0-100%)
                uint8_t joy_y = msg.byte[1];      // Y-axis for servo (0-100%)
                uint8_t joy_btn = msg.byte[2];    // Joystick button state (0=released, 1=pressed)
                
                // ========== SOLENOID CONTROL (Joystick Button) ==========
                // Fire solenoid on joystick button press (rising edge detection)
                if (joy_btn && !last_button) {
                    printf("FIRE! Solenoid activated\n");
                    solenoid_fire(50);  // 50ms pulse
                }
                last_button = joy_btn;
                
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
                }
                
                // ========== SERVO CONTROL (Y-axis) ==========
                // Invert Y-axis: Y=0 (top) → 100% (max angle), Y=100 (bottom) → 0% (min angle)
                uint8_t servo_percent = 100 - joy_y;
                
                // Only update if changed significantly
                if (abs(joy_y - last_y) > 1) {
                    servo_set_position(servo_percent);
                    last_y = joy_y;
                }
            }
        }
    }
}


/**
 * Motor Calibration Routine
 * 
 * This function calibrates the motor's travel range by:
 * 1. Waiting for user to manually center the motor
 * 2. Pressing the joystick button to start calibration
 * 3. Driving slowly to one side for 3 seconds (find min)
 * 4. Driving slowly to the other side for 3 seconds (find max)
 * 5. Calculating the mapping between joystick and encoder values
 * 6. Running normal PI control with the calibrated range
 */
void task8_motor_calibration(void) {
    printf("=== Motor Calibration ===\n");
    
    // Initialize peripherals
    servo_init();
    if (!encoder_init()) {
        printf("ERROR: Failed to initialize encoder!\n");
        return;
    }
    
    if (!motor_init()) {
        printf("ERROR: Failed to initialize motor!\n");
        return;
    }
    
    // Initialize solenoid
    solenoid_init();
    printf("Solenoid initialized\n");
    
    // Initialize CAN
    uint32_t working_can_br = 0x00290165;
    can_init((CanInit){.brp=20, .propag=2, .phase1=7, .phase2=6, .sjw=1, .smp=0}, 0);
    CAN0->CAN_BR = working_can_br;
    time_spinFor(msecs(100));
    
    printf("\n*** STEP 1: MANUAL CENTERING ***\n");
    printf("Center the motor, then PRESS JOYSTICK BUTTON\n\n");
    
    // Wait for button press
    bool button_pressed = false;
    uint32_t msg_count = 0;
    while (!button_pressed) {
        CanMsg msg;
        if (can_rx(&msg)) {
            msg_count++;
            if (msg.id == 0x00 && msg.length >= 3) {
                uint8_t button = msg.byte[2];
                if (button != 0) {
                    button_pressed = true;
                    printf("Button pressed! Starting...\n");
                }
            }
        }
    }
    
    // Reset encoder at center
    encoder_reset();
    time_spinFor(msecs(500));
    
    // Find LEFT limit
    printf("Finding LEFT limit (3s)...\n");
    motor_set_signed(-30);
    time_spinFor(msecs(3000));
    motor_set_signed(0);
    
    int16_t min_encoder = encoder_read();
    printf("LEFT: %d\n", min_encoder);
    time_spinFor(msecs(500));
    
    // Find RIGHT limit
    printf("Finding RIGHT limit (6s)...\n");
    motor_set_signed(30);
    time_spinFor(msecs(3000));
    motor_set_signed(0);
    
    int16_t max_encoder = encoder_read();
    printf("RIGHT: %d\n", max_encoder);
    time_spinFor(msecs(500));
    
    // Calculate calibration
    int16_t encoder_range = max_encoder - min_encoder;
    int16_t encoder_center = (max_encoder + min_encoder) / 2;
    float scale_factor = (float)encoder_range / 100.0f;
    
    printf("\nCALIBRATION COMPLETE\n");
    printf("Range: %d to %d (center: %d)\n", min_encoder, max_encoder, encoder_center);
    printf("Scale: %.2f\n\n", scale_factor);
    
    // Return to center
    printf("Returning to center...\n");
    uint32_t start_time = time_now();
    uint32_t timeout = 10000;
    
    while ((time_now() - start_time) < timeout) {
        int16_t pos = encoder_read();
        int16_t error = encoder_center - pos;
        
        if (abs(error) < 20) {
            motor_set_signed(0);
            printf("Centered at %d\n\n", pos);
            break;
        }
        
        int8_t speed = -(int8_t)(error / 10);
        if (speed > 50) speed = 50;
        if (speed < -50) speed = -50;
        
        motor_set_signed(speed);
        time_spinFor(msecs(50));
    }
    
    motor_set_signed(0);
    
    time_spinFor(msecs(1000));
    
    // Start PI control
    printf("Starting PI control...\n");
    motor_pi_init(0.06f, 0.001f);
    motor_pi_set_target(encoder_center);
    
    uint32_t last_debug = time_now();
    uint8_t last_y = 255;
    uint8_t last_button = 0;
    
    // Initialize IR sensor goal detection
    ir_sensor_init();
    uint16_t ir_baseline = ir_sensor_calibrate();
    uint16_t ir_threshold = ir_baseline * 7 / 10;
    printf("IR sensor initialized: baseline=%d mV, threshold=%d mV\n", ir_baseline, ir_threshold);
    
    // Debouncing: require beam to be broken for multiple consecutive readings
    #define DEBOUNCE_COUNT 2  // Must see beam broken 5 times in a row
    uint8_t beam_broken_counter = 0;

    // Scoring: increment when beam stays intact for 2s continuously
    uint32_t last_time = time_now();
    uint32_t intact_accumulator_ms = 0;
    uint32_t local_score = 0;
    
    while (1) {
        uint32_t now = time_now();
        uint32_t dt = (now >= last_time) ? (now - last_time) : 0;
        last_time = now;

        // ========== BEAM STATE HANDLING WITH DEBOUNCING ==========
        bool beam_currently_broken = ir_sensor_is_beam_broken(ir_threshold);
        
        if (beam_currently_broken) {
            // Increment counter when beam is broken
            beam_broken_counter++;
            
            // Only trigger game over after consecutive broken readings
            if (beam_broken_counter >= DEBOUNCE_COUNT) {
                // Beam is definitely broken - game over
                if (local_score > 0) {
                    printf("\n*** Beam broken - final score: %lu ***\n", local_score);
                } else {
                    printf("\n*** Beam broken - no score ***\n");
                }
                printf("*** GAME OVER - Returning to menu ***\n");
                
                // Send game over message to Node 1 (CAN ID 0x01)
                CanMsg game_over_msg;
                game_over_msg.id = 0x01;
                game_over_msg.length = 4;
                game_over_msg.byte[0] = 0xFF;  // Game over flag
                game_over_msg.byte[1] = (local_score >> 24) & 0xFF;
                game_over_msg.byte[2] = (local_score >> 16) & 0xFF;
                game_over_msg.byte[3] = (local_score >> 8) & 0xFF;
                can_tx(game_over_msg);
                
                motor_set_signed(0);  // Stop motor
                return;  // Exit back to menu
            }
        } else {
            // Beam is intact - reset counter and accumulate score time
            beam_broken_counter = 0;
            intact_accumulator_ms += dt;
            
            if (intact_accumulator_ms >= 2000) {
                // Award one score point for each full 2s of uninterrupted play
                intact_accumulator_ms -= 2000;
                local_score++;
                ir_sensor_increment_score();
                printf("+++ Score +1  (total: %lu) +++\n", local_score);
            }
        }

        // ========== CAN MESSAGE HANDLING ==========
        CanMsg msg;
        if (can_rx(&msg)) {                
            if (msg.id == 0x00 && msg.length >= 3) {
                uint8_t joy_x = msg.byte[0];      // X-axis for motor (0-100%)
                uint8_t joy_y = msg.byte[1];      // Y-axis for servo (0-100%)
                uint8_t joy_btn = msg.byte[2];    // Joystick button state (0=released, 1=pressed)
                
                // ========== SOLENOID CONTROL (Joystick Button) ==========
                // Fire solenoid on joystick button press (rising edge detection)
                if (joy_btn && !last_button) {
                    printf("FIRE! Solenoid activated\n");
                    solenoid_fire(50);  // 50ms pulse
                }
                last_button = joy_btn;
                
                // ========== MOTOR CONTROL (PI Position Control) ==========
                int16_t target = min_encoder + (int16_t)(joy_x * scale_factor);
                motor_pi_set_target(target);
                
                int16_t position = encoder_read();
                int8_t motor_cmd = motor_pi_update(position);
                motor_set_signed(motor_cmd);
                
                // ========== SERVO CONTROL (Y-axis) ==========
                int8_t joy_y_centered = (int8_t)(joy_y - 50);
                if (joy_y_centered > -5 && joy_y_centered < 5) {
                    joy_y = 50;
                }
                
                uint8_t servo_percent = 100 - joy_y;
                
                if (abs((int8_t)(joy_y - last_y)) > 3) {
                    servo_set_position(servo_percent);
                    last_y = joy_y;
                }
                
                // Debug every 500ms
                if ((now - last_debug) >= 500) {
                    int16_t error = target - position;
                    printf("Joy:%3d Tgt:%4d Pos:%4d Err:%4d Mot:%4d%%\n", 
                           joy_x, target, position, error, motor_cmd);
                    last_debug = now;
                }
            }
        }
        
        // Rate limiting: 20ms per loop iteration (50Hz control loop)
        time_spinFor(msecs(20));
    }
}
