/*
 * task8.h - Motor Control and Encoder Tests
 */

#ifndef TASK8_H
#define TASK8_H

// Test encoder reading - verify quadrature decoder works
void task8_encoder_test(void);

// Test motor control with joystick (Task 8 Step 3 - open loop)
void task8_joystick_motor_control(void);

// PI position controller with joystick (Task 8 Step 4 - closed loop)
void task8_pi_position_control(void);

// Diagnostic test - simple P-only control to understand system
void task8_diagnostic_p_control(void);

// Incremental PI control - starts from working baseline
void task8_incremental_pi_control(void);

// Simple PI control - minimal implementation
void task8_simple_pi(void);

// Motor calibration - find encoder min/max range
void task8_motor_calibration(void);



#endif
