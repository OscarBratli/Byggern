/*
 * task7.h - PWM Servo Control Tests
 */

#ifndef TASK7_H
#define TASK7_H

// Test PWM signal range (for oscilloscope verification)
void task7_pwm_range_test(void);

// Control servo with joystick via CAN bus
void task7_joystick_servo_control(void);

// Test IR sensor - check voltage and beam break detection
void task7_ir_sensor_test(void);

#endif
