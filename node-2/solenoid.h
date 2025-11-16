#ifndef SOLENOID_H
#define SOLENOID_H

#include <stdint.h>

/**
 * @brief Initialize the solenoid control pin
 * 
 * Configures PB25 (digital pin 2) as output for solenoid relay control.
 * The solenoid requires 12V to activate and is controlled via a relay
 * with a PNP transistor driver (LOW = ON, HIGH = OFF).
 */
void solenoid_init(void);

/**
 * @brief Fire the solenoid for a brief pulse
 * 
 * Activates the solenoid for a short duration to hit the ping pong ball.
 * The pulse duration is just long enough to strike the ball without
 * damaging the solenoid.
 * 
 * @param duration_ms Duration of the solenoid pulse in milliseconds (default: 50ms)
 */
void solenoid_fire(uint16_t duration_ms);

/**
 * @brief Manually control the solenoid state
 * 
 * @param active 1 to activate solenoid, 0 to deactivate
 * 
 * WARNING: Prolonged activation may damage the solenoid. Use solenoid_fire() instead.
 */
void solenoid_set(uint8_t active);

#endif // SOLENOID_H
