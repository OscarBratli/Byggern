/*
 * motor.c - Motor Driver for ATSAM3X8E using A3959
 * 
 * Implementation of DC motor control using A3959 in phase/enable mode
 */

#include "motor.h"
#include "sam.h"
#include "uart.h"
#include <stdio.h>
#include <stdlib.h>


 //NOTE TO SELF : DEBOUNCE -> 3  to redue noise on encoder 
// Pin definitions for TTK4155 Motor Shield
// According to A3959 phase/enable mode and TTK4155 Motor Shield pinout:
// - PWM on ENABLE pin (PB12 = PWMH0 = Arduino Due pin 21/22) for speed control
// - Digital signal on PHASE/DIR pin (PC23 = Arduino Due pin 7) for direction
// - No chip enable needed - A3959 is always enabled when board is powered

#define MOTOR_PWM_CHANNEL 0
#define MOTOR_DIR_PIN PIO_PC23

// Motor state
static uint8_t current_speed = 0;
static motor_direction_t current_direction = MOTOR_DIR_RIGHT;

// PI controller state
static float pi_kp = 0.0f;
static float pi_ki = 0.0f;
static int16_t pi_target = 0;
static float pi_integral = 0.0f;

bool motor_init(void) {
    // 1. Enable PWM peripheral clock
    PMC->PMC_PCER1 |= (1 << (ID_PWM - 32));
    
    // 2. Configure PIO for PWM output (PB12 = PWMH0 = ENABLE pin)
    PIOB->PIO_PDR |= PIO_PB12;     // Disable PIO control
    PIOB->PIO_ABSR |= PIO_PB12;    // Select peripheral B (PWM)
    
    // 3. Configure DIR pin (PC23 = PHASE/DIR pin)
    PMC->PMC_PCER0 |= (1 << ID_PIOC);
    PIOC->PIO_PER |= MOTOR_DIR_PIN;
    PIOC->PIO_OER |= MOTOR_DIR_PIN;
    PIOC->PIO_CODR |= MOTOR_DIR_PIN;
    
    // 4. Configure PWM for motor speed control (20 kHz)
    PWM->PWM_DIS = (1 << MOTOR_PWM_CHANNEL);
    
    PWM->PWM_CLK = PWM_CLK_PREA(6) | PWM_CLK_DIVA(1);
    PWM->PWM_CH_NUM[MOTOR_PWM_CHANNEL].PWM_CMR = PWM_CMR_CPRE_CLKA;
    PWM->PWM_CH_NUM[MOTOR_PWM_CHANNEL].PWM_CPRD = 66;
    PWM->PWM_CH_NUM[MOTOR_PWM_CHANNEL].PWM_CDTY = 66;
    
    PWM->PWM_ENA = (1 << MOTOR_PWM_CHANNEL);
    
    current_speed = 0;
    current_direction = MOTOR_DIR_RIGHT;
    
    return true;
}

void motor_set(int8_t speed, motor_direction_t direction) {
    // Clamp speed
    if (speed < 0) speed = 0;
    if (speed > 70) speed = 70;
    
    current_speed = speed;
    current_direction = direction;
    
    // Set direction pin (PHASE/DIR)
    // For A3959 phase/enable mode:
    // PHASE LOW = one direction, PHASE HIGH = other direction
    if (direction == MOTOR_DIR_LEFT) {
        PIOC->PIO_CODR = MOTOR_DIR_PIN;  // Clear (LOW) for LEFT
    } else {
        PIOC->PIO_SODR = MOTOR_DIR_PIN;  // Set (HIGH) for RIGHT
    }
    
    // Set PWM duty cycle for !ENABLE pin (ACTIVE LOW!)
    // !ENABLE is ACTIVE LOW: LOW = enabled, HIGH = disabled
    // So we need to INVERT the duty cycle:
    // - speed = 0%   → duty = 100% (pin HIGH = disabled = stopped)
    // - speed = 100% → duty = 0%   (pin LOW = enabled = full speed)
    uint32_t period = PWM->PWM_CH_NUM[MOTOR_PWM_CHANNEL].PWM_CPRD;
    uint32_t duty = (period * (100 - speed)) / 100;  // INVERTED!
    
    PWM->PWM_CH_NUM[MOTOR_PWM_CHANNEL].PWM_CDTYUPD = duty;
    
    // Debug output removed - too spammy, use task8 debug instead
}

void motor_set_signed(int8_t signed_speed) {
    // Clamp to -100 to +100
    if (signed_speed < -100) signed_speed = -100;
    if (signed_speed > 100) signed_speed = 100;
    
    // Convert signed speed to direction + magnitude
    motor_direction_t direction;
    uint8_t speed;
    
    if (signed_speed < 0) {
        direction = MOTOR_DIR_LEFT;
        speed = abs(signed_speed);
    } else {
        direction = MOTOR_DIR_RIGHT;
        speed = signed_speed;
    }
    
    motor_set(speed, direction);
}

void motor_stop(void) {
    motor_set(0, current_direction);
}

uint8_t motor_get_speed(void) {
    return current_speed;
}

motor_direction_t motor_get_direction(void) {
    return current_direction;
}

// ========== PI Controller Functions ==========

void motor_pi_init(float kp, float ki) {
    pi_kp = kp;
    pi_ki = ki;
    pi_target = 0;  // Will be set by first joystick command
    pi_integral = 0.0f;
    printf("PI Controller: Kp=%.3f, Ki=%.5f\n", kp, ki);
}

void motor_pi_set_target(int16_t target) {
    pi_target = target;
}

int8_t motor_pi_update(int16_t position) {
    // Calculate error
    int16_t error = pi_target - position;
    
    // Update integral with anti-windup
    pi_integral += (float)error;
    
    // Anti-windup: Clamp integral to prevent it from growing too large
    float max_integral = 500.0f;  // Limit integral contribution
    if (pi_integral > max_integral) pi_integral = max_integral;
    if (pi_integral < -max_integral) pi_integral = -max_integral;
    
    // Reset integral when very close to target (fine positioning)
    if (abs(error) < 10) {
        pi_integral = 0.0f;
    }
    
    // Calculate output: PI control law
    float output = (float)error * pi_kp + pi_integral * pi_ki;
    
    // INVERT output because motor direction is opposite to encoder direction!
    output = -output;
    
    // Limit output to motor range
    if (output > 100.0f) output = 100.0f;
    if (output < -100.0f) output = -100.0f;
    
    return (int8_t)output;
}
