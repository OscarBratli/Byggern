/*
 * pwm.h - PWM Driver for Arduino Due (ATSAM3X8E)
 * 
 * This driver uses the SAM3X PWM Controller to generate PWM signals
 * for servo control with built-in safety features.
 * 
 * Servo PWM Requirements:
 * - Period: 20ms (50Hz)
 * - Pulse width: 0.9ms - 2.1ms
 * - Center position: 1.5ms
 */

#ifndef PWM_H
#define PWM_H

#include <stdint.h>
#include <stdbool.h>

// PWM Configuration Constants
#define PWM_FREQUENCY_HZ        50      // 50Hz for servo (20ms period)
#define PWM_PERIOD_MS           20      // 20ms period
#define PWM_PERIOD_US           20000   // 20ms = 20000us

// Servo Safe Range (in microseconds)
#define PWM_SERVO_MIN_US        900     // 0.9ms minimum pulse width
#define PWM_SERVO_MAX_US        2100    // 2.1ms maximum pulse width
#define PWM_SERVO_CENTER_US     1500    // 1.5ms center position

// PWM Channel Selection  
#define PWM_SERVO_CHANNEL       1       // Use PWM channel 1 (pin PB13/PWMH1, Arduino D21)

// PWM Channel Pin: PB16 is Arduino Due pin 21 (PWMH0)
// This corresponds to the servo signal pin on the motor shield

/**
 * @brief Initialize the PWM controller for servo control
 * 
 * Configures PWM channel 0 on pin PB16 (Arduino Due pin 21)
 * Sets up 50Hz frequency with proper clock divider
 * 
 * @return true if initialization successful, false otherwise
 */
bool pwm_init(void);

/**
 * @brief Set PWM pulse width in microseconds with safety clamping
 * 
 * Automatically clamps the pulse width to safe servo range (900-2100us)
 * This prevents servo damage from out-of-range signals
 * 
 * @param pulse_width_us Desired pulse width in microseconds
 * @return true if pulse width was set, false on error
 */
bool pwm_set_pulse_width_us(uint16_t pulse_width_us);

/**
 * @brief Set PWM duty cycle as percentage (0-100%)
 * 
 * Maps percentage to pulse width:
 * - 0% = 900us (minimum)
 * - 50% = 1500us (center)
 * - 100% = 2100us (maximum)
 * 
 * @param duty_percent Duty cycle percentage (0-100)
 * @return true if duty cycle was set, false on error
 */
bool pwm_set_duty_percent(uint8_t duty_percent);

/**
 * @brief Get current PWM pulse width in microseconds
 * 
 * @return Current pulse width in microseconds
 */
uint16_t pwm_get_pulse_width_us(void);

/**
 * @brief Disable PWM output (set to safe center position first)
 * 
 * Moves servo to center position before disabling output
 * This prevents sudden movements when re-enabling
 */
void pwm_disable(void);

/**
 * @brief Enable PWM output
 * 
 * Re-enables PWM output after being disabled
 */
void pwm_enable(void);

/**
 * @brief Print current PWM configuration and status
 * 
 * Useful for debugging - prints frequency, period, pulse width, etc.
 */
void pwm_print_status(void);

#endif // PWM_H
