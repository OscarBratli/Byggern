/**
 * @file ir_sensor.h
 * @brief IR beam break sensor driver for goal detection
 * 
 * Uses SAM3X ADC to read photodiode voltage.
 * The IR LED transmitter continuously emits IR light.
 * When the beam is broken (ball passes), photodiode voltage drops.
 */

#ifndef IR_SENSOR_H
#define IR_SENSOR_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Initialize the IR sensor system
 * 
 * Sets up:
 * - ADC channel for photodiode reading
 * - ADC configuration (12-bit, single-ended)
 * - Pin configuration
 * 
 * Hardware setup (per Figure 25):
 * - IR LED: +5V -> IR LED -> 47Ω -> GND
 * - IR Photodiode: +5V -> Photodiode -> R -> ADC pin -> Your circuit -> GND
 * - ADC channel 0 (PA2 = Arduino A7 on Due)
 * - ADC range: 0-3.3V
 */
void ir_sensor_init(void);

/**
 * @brief Read raw ADC value from IR photodiode
 * @return 12-bit ADC value (0-4095)
 *         Higher value = more IR light detected (beam intact)
 *         Lower value = less IR light (beam broken)
 */
uint16_t ir_sensor_read_raw(void);

/**
 * @brief Read IR sensor voltage in millivolts
 * @return Voltage in mV (0-3300)
 */
uint16_t ir_sensor_read_voltage_mv(void);

/**
 * @brief Check if IR beam is broken (goal detected)
 * @param threshold_mv Voltage threshold in mV (below = beam broken)
 * @return true if beam is broken (goal!), false if beam intact
 * 
 * Typical threshold: ~1500mV (adjust based on your circuit)
 */
bool ir_sensor_is_beam_broken(uint16_t threshold_mv);

/**
 * @brief Calibrate sensor by reading ambient light level
 * @return Baseline voltage when beam is intact (mV)
 * 
 * Call this during setup with beam intact to establish baseline.
 * Then set threshold to ~50-70% of this value.
 */
uint16_t ir_sensor_calibrate(void);

/**
 * @brief Get current goal/score count
 * @return Number of goals detected since initialization
 */
uint32_t ir_sensor_get_score(void);

/**
 * @brief Reset goal/score count to zero
 */
void ir_sensor_reset_score(void);

/**
 * @brief Increment goal/score count
 * 
 * Call this when a goal is detected.
 * Typically used internally but can be called manually if needed.
 */
void ir_sensor_increment_score(void);

#endif // IR_SENSOR_H
