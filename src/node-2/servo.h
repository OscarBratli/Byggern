/*
 * servo.h - Servo Control Driver for Arduino Due
 * 
 * High-level servo driver that uses the PWM module to control
 * servo position with safety features and position mapping.
 */

#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>
#include <stdbool.h>

// Servo position range (0-100%)
#define SERVO_POSITION_MIN      0       // Minimum position (0%)
#define SERVO_POSITION_MAX      100     // Maximum position (100%)
#define SERVO_POSITION_CENTER   50      // Center position (50%)

/**
 * @brief Initialize servo control system
 * 
 * Initializes PWM controller and sets servo to center position
 * 
 * @return true if initialization successful, false otherwise
 */
bool servo_init(void);

/**
 * @brief Set servo position as percentage (0-100%)
 * 
 * Maps position to pulse width:
 * - 0% = 900us (full left)
 * - 50% = 1500us (center)
 * - 100% = 2100us (full right)
 * 
 * SAFETY: Automatically clamps position to valid range
 * 
 * @param position Desired position (0-100%)
 * @return true if position was set, false on error
 */
bool servo_set_position(uint8_t position);

/**
 * @brief Set servo position from joystick X value
 * 
 * Converts joystick X position (0-100%) to servo position
 * Provides convenient interface for joystick control
 * 
 * @param joystick_x Joystick X position (0-100%)
 * @return true if position was set, false on error
 */
bool servo_set_from_joystick_x(uint8_t joystick_x);

/**
 * @brief Set servo position from joystick Y value
 * 
 * Converts joystick Y position (0-100%) to servo position
 * Provides convenient interface for joystick control
 * 
 * @param joystick_y Joystick Y position (0-100%)
 * @return true if position was set, false on error
 */
bool servo_set_from_joystick_y(uint8_t joystick_y);

/**
 * @brief Get current servo position
 * 
 * @return Current position (0-100%)
 */
uint8_t servo_get_position(void);

/**
 * @brief Move servo to center position
 * 
 * Convenience function to center the servo
 */
void servo_center(void);

/**
 * @brief Disable servo (moves to center first for safety)
 * 
 * SAFETY: Moves servo to center position before disabling
 * to prevent sudden movements when re-enabling
 */
void servo_disable(void);

/**
 * @brief Enable servo output
 * 
 * Re-enables servo control after being disabled
 */
void servo_enable(void);

/**
 * @brief Print current servo status
 * 
 * Displays position, pulse width, and configuration
 */
void servo_print_status(void);

#endif // SERVO_H
