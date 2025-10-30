/*
 * pi_controller.h - PID Position Controller
 * 
 * PID controller for motor position control
 */

#ifndef PI_CONTROLLER_H
#define PI_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>

// PID Controller structure
typedef struct {
    float Kp;              // Proportional gain
    float Ki;              // Integral gain
    float Kd;              // Derivative gain
    float integral;        // Integral accumulator
    float integral_max;    // Anti-windup limit for integral
    int16_t target;        // Target position (encoder counts)
    int16_t last_error;    // Last error (for derivative calculation)
} pi_controller_t;

/**
 * @brief Initialize PID controller
 * 
 * @param controller Pointer to controller structure
 * @param Kp Proportional gain
 * @param Ki Integral gain
 * @param Kd Derivative gain
 * @param integral_max Maximum integral value for anti-windup
 */
void pi_controller_init(pi_controller_t *controller, float Kp, float Ki, float Kd, float integral_max);

/**
 * @brief Reset controller state (clear integral)
 * 
 * @param controller Pointer to controller structure
 */
void pi_controller_reset(pi_controller_t *controller);

/**
 * @brief Set target position
 * 
 * @param controller Pointer to controller structure
 * @param target Target position in encoder counts
 */
void pi_controller_set_target(pi_controller_t *controller, int16_t target);

/**
 * @brief Update controller and calculate motor command
 * 
 * @param controller Pointer to controller structure
 * @param current_position Current encoder position
 * @return Motor speed command (-100 to +100)
 */
int8_t pi_controller_update(pi_controller_t *controller, int16_t current_position);

/**
 * @brief Get current error
 * 
 * @param controller Pointer to controller structure
 * @return Current position error
 */
int16_t pi_controller_get_error(pi_controller_t *controller);

#endif // PI_CONTROLLER_H
