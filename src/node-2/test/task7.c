/*
 * task7.c - PWM Servo Control with Joystick
 */

#include <stdio.h>
#include "sam.h"
#include "../uart.h"
#include "../pwm.h"
#include "../servo.h"
#include "../can.h"
#include "../ir_sensor.h"

static void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 10000; i++) {
        __asm__("nop");
    }
}

void task7_pwm_range_test(void) {
    printf("PWM Range Verification Test\n");
    printf("Connect oscilloscope to Pin 21\n\n");
    
    pwm_init();
    
    uint16_t pulses[] = {900, 1200, 1500, 1800, 2100};
    const char* names[] = {"MIN", "25%", "CENTER", "75%", "MAX"};
    
    while (1) {
        for (int i = 0; i < 5; i++) {
            pwm_set_pulse_width_us(pulses[i]);
            printf("%s: %.1f ms (%d us)\n", names[i], pulses[i]/1000.0, pulses[i]);
            delay_ms(3000);
        }
        printf("\n");
    }
}

void task7_joystick_servo_control(void) {
    printf("\n=== Task 7: Joystick Servo Control ===\n");
    printf("Servo controls shooting mechanism angle\n");
    printf("Joystick Y-axis controls servo position (INVERTED)\n");
    printf("Y=0%%   (top)    -> 2.1ms (max angle)\n");
    printf("Y=50%%  (center) -> 1.5ms (mid angle)\n");
    printf("Y=100%% (bottom) -> 0.9ms (min angle)\n\n");
    
    // Initialize servo (starts at center)
    servo_init();
    printf("Servo initialized at center position (1.5ms)\n\n");
    
    // Initialize CAN to receive joystick data
    // Use the proven working CAN_BR configuration from Task 6
    uint32_t working_can_br = 0x00290165;  // Proven working value
    
    can_init((CanInit){
        .brp = 20, .propag = 2, .phase1 = 7, .phase2 = 6, .sjw = 1, .smp = 0
    }, 0);
    
    // Override with exact working register value
    CAN0->CAN_BR = working_can_br;
    
    printf("CAN initialized (CAN_BR = 0x%08X) - waiting for joystick data...\n\n", working_can_br);
    
    CanMsg msg;
    uint8_t last_x_percent = 50; // Track changes
    
    while (1) {
        // Check if we received a CAN message
        if (can_rx(&msg)) {
            // Check if it's joystick data (ID = 0x00)
            if (msg.id == 0x00 && msg.length >= 2) {
                // Joystick data format: [X, Y, ?, Button, Slider]
                uint8_t y_raw = msg.byte[1];  // Raw Y position (0-100) - ALREADY in percent!
                
                // Y is already 0-100%, just need to invert it
                // Y=0 (top) should give 100% (2.1ms), Y=100 (bottom) should give 0% (0.9ms)
                uint8_t servo_percent = 100 - y_raw;
                
                // Only update if position changed significantly (reduce jitter)
                // Reduced threshold from 2% to 1% for faster response
                if (abs(servo_percent - last_x_percent) > 1) {
                    // Set servo position based on inverted joystick Y-axis
                    servo_set_position(servo_percent);
                    
                    // Optional: Uncomment for debugging
                    // uint16_t pulse_us = pwm_get_pulse_width_us();
                    // printf("Y: %d%% -> Servo: %d%% = %.2fms\n", y_raw, servo_percent, pulse_us/1000.0);
                    
                    last_x_percent = servo_percent;
                }
            }
        }
        // No delay - poll as fast as possible for instant response
    }
}

/**
 * Test IR sensor - read voltage and detect beam breaks
 * This should be run BEFORE connecting to microcontroller
 * Use a multimeter to verify signal is in 0-3.3V range
 */
void task7_ir_sensor_test(void) {
    
    // Initialize IR sensor
    ir_sensor_init();
    
    // Calibrate to get baseline (beam intact)
    uint16_t baseline = ir_sensor_calibrate();
    
    // Set threshold to 70% of baseline
    uint16_t threshold = baseline * 7 / 10;
    // Reset score at start
    ir_sensor_reset_score();
    
    bool was_broken = false;
    bool debounced_state = false;  // Stable debounced state
    
    // Debouncing variables
    #define DEBOUNCE_COUNT 5  // Must see same state 5 times in a row
    uint8_t broken_count = 0;
    uint8_t intact_count = 0;
    
    while (1) {
        // Read current sensor values
        uint16_t raw = ir_sensor_read_raw();
        uint16_t voltage = ir_sensor_read_voltage_mv();
        bool is_broken = ir_sensor_is_beam_broken(threshold);
        
        // Debounce the beam state
        if (is_broken) {
            broken_count++;
            intact_count = 0;
            if (broken_count >= DEBOUNCE_COUNT) {
                broken_count = DEBOUNCE_COUNT;  // Cap the counter
                debounced_state = true;
            }
        } else {
            intact_count++;
            broken_count = 0;
            if (intact_count >= DEBOUNCE_COUNT) {
                intact_count = DEBOUNCE_COUNT;  // Cap the counter
                debounced_state = false;
            }
        }
        
        // Detect rising edge on debounced state (beam broken -> intact)
        if (was_broken && !debounced_state) {
            ir_sensor_increment_score();
            printf("\n*** GOAL #%lu DETECTED! ***\n\n", ir_sensor_get_score());
        }
        
        // Print status every ~100ms
        static uint32_t print_counter = 0;
        if (print_counter++ % 10000 == 0) {
            printf("ADC: %4u (0x%03X) | Voltage: %4u mV | Status: %s | Debounce: %u/%u | Score: %lu\n", 
                   raw, raw, voltage, debounced_state ? "BROKEN" : "INTACT",
                   is_broken ? broken_count : intact_count, DEBOUNCE_COUNT, ir_sensor_get_score());
        }
        
        was_broken = debounced_state;
        
        // Small delay for readability
        for (volatile int i = 0; i < 1000; i++);
    }
}

