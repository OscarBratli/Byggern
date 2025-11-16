/*
 * encoder.h - Quadrature Encoder Driver for ATSAM3X8E
 * 
 * Uses TC2 (Timer Counter 2) in quadrature decoder mode to read
 * motor encoder position and direction.
 * 
 * Hardware connections:
 * - Channel A (TIOA6): PC25 (Arduino Due pin 5)
 * - Channel B (TIOB6): PC26 (Arduino Due pin 4)
 */

#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>
#include <stdbool.h>

// Encoder resolution (pulses per revolution)
// RE30E encoder: 360 pulses per revolution in quadrature mode (90 slots * 4)
#define ENCODER_PPR 360

/**
 * @brief Initialize encoder using TC2 in quadrature decoder mode
 * 
 * Configures TC2 Channel 0 (TC6) for quadrature decoding:
 * - TIOA6 (PC25) = Channel A
 * - TIOB6 (PC26) = Channel B
 * - Position mode with XC0 clock (TIOA/TIOB edges)
 * 
 * @return true if initialization successful
 */
bool encoder_init(void);

/**
 * @brief Read current encoder position (raw counter value)
 * 
 * @return 16-bit position counter value
 */
int16_t encoder_read(void);

/**
 * @brief Reset encoder position to zero
 */
void encoder_reset(void);

/**
 * @brief Get encoder position in number of revolutions
 * 
 * @return Position as fraction of full revolution (can be negative)
 */
float encoder_get_revolutions(void);

/**
 * @brief Get encoder direction
 * 
 * @return true if rotating forward (positive direction)
 */
bool encoder_get_direction(void);

/**
 * @brief Print encoder status for debugging
 * 
 * Prints current position, direction, and revolution count
 */
void encoder_print_status(void);

#endif // ENCODER_H
