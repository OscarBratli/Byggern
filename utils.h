
/**
 * This module contain a collection of utility functions and macros
 */

#pragma once

// Clock speed of the MCU (in Hz)
#define FOSC 4915200

// bit manipulation for pin in/out
#define set_bit(reg, bit) (reg |= (1 << bit))
#define clear_bit(reg, bit) (reg &= ~(1 << bit))
#define test_bit(reg, bit) (reg & (1 << bit))