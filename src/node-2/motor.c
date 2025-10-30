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

bool motor_init(void) {
    printf("\n=== Initializing Motor Driver (A3959) ===\n");
    printf("Mode: Phase/Enable - PWM on ENABLE, digital on DIR\n");
    
    // 1. Enable PWM peripheral clock
    PMC->PMC_PCER1 |= (1 << (ID_PWM - 32));
    printf("- PWM clock enabled\n");
    
    // 2. Configure PIO for PWM output (PB12 = PWMH0 = ENABLE pin)
    PIOB->PIO_PDR |= PIO_PB12;     // Disable PIO control (let PWM peripheral control it)
    PIOB->PIO_ABSR |= PIO_PB12;    // Select peripheral B (PWM)
    printf("- PB12 (PWMH0) configured for PWM -> ENABLE pin\n");
    
    // 3. Configure DIR pin (PC23 = PHASE/DIR pin)
    PMC->PMC_PCER0 |= (1 << ID_PIOC);  // Enable PIOC clock
    PIOC->PIO_PER |= MOTOR_DIR_PIN;     // Enable PIO control
    PIOC->PIO_OER |= MOTOR_DIR_PIN;     // Output enable
    PIOC->PIO_CODR |= MOTOR_DIR_PIN;    // Set LOW initially
    printf("- PC23 configured as digital output -> PHASE/DIR pin\n");
    
    // 4. Configure PWM for motor speed control
    // Disable PWM channel during configuration
    PWM->PWM_DIS = (1 << MOTOR_PWM_CHANNEL);
    
    // PWM frequency: 20 kHz (50us period) - motor acts as low pass filter
    // Clock: MCK/64 = 84MHz/64 = 1.3125 MHz
    // Period = 1.3125 MHz / 20 kHz = 65.625 ≈ 66 ticks
    
    PWM->PWM_CLK = PWM_CLK_PREA(6)     // Divider = 2^6 = 64
                 | PWM_CLK_DIVA(1);    // Linear divider = 1
    
    // Channel configuration
    PWM->PWM_CH_NUM[MOTOR_PWM_CHANNEL].PWM_CMR = 
        PWM_CMR_CPRE_CLKA;              // Use CLKA as clock source
    
    // Set period (20 kHz = 50us)
    PWM->PWM_CH_NUM[MOTOR_PWM_CHANNEL].PWM_CPRD = 66;
    
    // Set initial duty cycle to 100% (stopped - !ENABLE is active low!)
    // 100% duty = pin always HIGH = motor disabled = STOPPED
    PWM->PWM_CH_NUM[MOTOR_PWM_CHANNEL].PWM_CDTY = 66;
    
    // Enable PWM channel
    PWM->PWM_ENA = (1 << MOTOR_PWM_CHANNEL);
    
    printf("- PWM configured: 20kHz, duty=100%% (pin HIGH = STOPPED)\n");
    printf("- A3959 enabled by board power (no chip select needed)\n");
    printf("=====================================\n");
    printf("IMPORTANT: Verify these physical connections:\n");
    printf("  1. Arduino Due Pin D21/D22 (PB12) -> Motor Shield ENABLE/PWM\n");
    printf("  2. Arduino Due Pin D7 (PC23) -> Motor Shield PHASE/DIR\n");
    printf("  3. Motor+ and Motor- (2mm plugs) -> Motor\n");
    printf("  4. 12V power to motor shield\n");
    printf("=====================================\n\n");
    
    current_speed = 0;
    current_direction = MOTOR_DIR_RIGHT;
    
    return true;
}

void motor_set(int8_t speed, motor_direction_t direction) {
    // Clamp speed to 0-100
    if (speed < 0) speed = 0;
    if (speed > 100) speed = 100;
    
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
