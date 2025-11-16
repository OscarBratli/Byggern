/*
 * motor.h - Motor Driver for ATSAM3X8E using A3959
 * 
 * Controls DC motor using A3959 motor driver in phase/enable mode:
 * - PWM signal controls speed (0-100%)
 * - DIR signal controls direction (0=left, 1=right)
 * - EN signal enables/disables motor
 * 
 * Hardware connections (typical TTK4155 Motor Shield):
 * - PWM/Enable: Use PWM from PWM Controller
 * - DIR/Phase: Digital output pin
 * - EN: Digital output pin (optional, can be tied high on shield)
 */

#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>
#include <stdbool.h>

// Motor direction constants
typedef enum {
    MOTOR_DIR_LEFT = 0,
    MOTOR_DIR_RIGHT = 1
} motor_direction_t;

/**
 * @brief Initialize motor driver
 * 
 * Configures:
 * - PWM for speed control (using PWM Controller)
 * - DIR pin for direction control
 * - EN pin for enable (if used)
 * 
 * @return true if initialization successful
 */
bool motor_init(void);

/**
 * @brief Set motor speed and direction
 * 
 * @param speed Speed percentage (0-100), 0 = stop
 * @param direction Motor direction (MOTOR_DIR_LEFT or MOTOR_DIR_RIGHT)
 */
void motor_set(int8_t speed, motor_direction_t direction);

/**
 * @brief Initialize PI controller for position control
 * 
 * @param kp Proportional gain
 * @param ki Integral gain
 */
void motor_pi_init(float kp, float ki);

/**
 * @brief Set target position for PI controller
 * 
 * @param target Target position in encoder counts
 */
void motor_pi_set_target(int16_t target);

/**
 * @brief PI controller update - call at regular intervals
 * 
 * @param position Current position in encoder counts
 * @return Motor command (-100 to +100)
 */
int8_t motor_pi_update(int16_t position);

/**
 * @brief Set motor speed with signed value
 * 
 * Negative = left, Positive = right, 0 = stop
 * 
 * @param signed_speed Speed from -100 to +100
 */
void motor_set_signed(int8_t signed_speed);

/**
 * @brief Stop motor
 */
void motor_stop(void);

/**
 * @brief Get current motor speed
 * 
 * @return Current speed (0-100%)
 */
uint8_t motor_get_speed(void);

/**
 * @brief Get current motor direction
 * 
 * @return Current direction
 */
motor_direction_t motor_get_direction(void);

#endif // MOTOR_H
