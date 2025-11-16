/*
 * servo.c - Servo Control Driver for Arduino Due
 * 
 * High-level servo driver that abstracts PWM details and provides
 * simple position-based control with safety features.
 */

#include "servo.h"
#include "pwm.h"
#include "uart.h"
#include <stdio.h>

// Global state
static uint8_t current_position = SERVO_POSITION_CENTER;
static bool servo_initialized = false;

/**
 * @brief Map position percentage to pulse width in microseconds
 * 
 * Maps servo position (0-100%) to pulse width (900-2100us)
 * Uses linear interpolation for smooth control
 */
static uint16_t servo_position_to_pulse_width(uint8_t position) {
    // SAFETY: Clamp position to valid range
    if (position > SERVO_POSITION_MAX) {
        position = SERVO_POSITION_MAX;
    }
    
    // Linear mapping: position % -> pulse width us
    // 0% -> 900us, 50% -> 1500us, 100% -> 2100us
    uint16_t pulse_range = PWM_SERVO_MAX_US - PWM_SERVO_MIN_US;  // 1200us
    uint16_t pulse_width = PWM_SERVO_MIN_US + 
                           ((uint32_t)position * pulse_range) / SERVO_POSITION_MAX;
    
    return pulse_width;
}

/**
 * @brief Map pulse width to position percentage
 * 
 * Reverse mapping from pulse width (900-2100us) to position (0-100%)
 */
static uint8_t servo_pulse_width_to_position(uint16_t pulse_width_us) {
    if (pulse_width_us <= PWM_SERVO_MIN_US) {
        return SERVO_POSITION_MIN;
    }
    if (pulse_width_us >= PWM_SERVO_MAX_US) {
        return SERVO_POSITION_MAX;
    }
    
    uint16_t pulse_range = PWM_SERVO_MAX_US - PWM_SERVO_MIN_US;
    uint8_t position = ((uint32_t)(pulse_width_us - PWM_SERVO_MIN_US) * 
                        SERVO_POSITION_MAX) / pulse_range;
    
    return position;
}

bool servo_init(void) {
    // Initialize PWM driver
    if (!pwm_init()) {
        printf("ERROR: Failed to initialize PWM driver!\n");
        return false;
    }
    
    // Set to center position for safety
    servo_initialized = true;
    servo_set_position(SERVO_POSITION_CENTER);
    
    return true;
}

bool servo_set_position(uint8_t position) {
    if (!servo_initialized) {
        return false;
    }
    
    // SAFETY: Clamp to valid position range
    uint8_t safe_position = position;
    if (safe_position > SERVO_POSITION_MAX) {
        safe_position = SERVO_POSITION_MAX;
    }
    
    // Convert position to pulse width
    uint16_t pulse_width = servo_position_to_pulse_width(safe_position);
    
    // Set PWM pulse width (this also has safety clamping)
    if (!pwm_set_pulse_width_us(pulse_width)) {
        return false;
    }
    
    current_position = safe_position;
    return true;
}

bool servo_set_from_joystick_x(uint8_t joystick_x) {
    // Joystick X is already 0-100%, so direct mapping
    return servo_set_position(joystick_x);
}

bool servo_set_from_joystick_y(uint8_t joystick_y) {
    // Joystick Y is already 0-100%, so direct mapping
    return servo_set_position(joystick_y);
}

uint8_t servo_get_position(void) {
    return current_position;
}

void servo_center(void) {
    servo_set_position(SERVO_POSITION_CENTER);
    printf("Servo centered at %d%%\n", SERVO_POSITION_CENTER);
}

void servo_disable(void) {
    if (!servo_initialized) {
        return;
    }
    
    printf("Disabling servo...\n");
    // PWM driver will center servo before disabling
    pwm_disable();
}

void servo_enable(void) {
    if (!servo_initialized) {
        return;
    }
    
    printf("Enabling servo...\n");
    pwm_enable();
}

void servo_print_status(void) {
    if (!servo_initialized) {
        printf("Servo not initialized\n");
        return;
    }
    
    printf("\n=== Servo Status ===\n");
    printf("Current position: %d%%\n", current_position);
    printf("Pulse width: %dus\n", pwm_get_pulse_width_us());
    
    // Calculate position description
    const char* position_desc;
    if (current_position < 30) {
        position_desc = "LEFT";
    } else if (current_position > 70) {
        position_desc = "RIGHT";
    } else {
        position_desc = "CENTER";
    }
    printf("Position: %s\n", position_desc);
    printf("====================\n\n");
}
