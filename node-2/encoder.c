/*
 * encoder.c - Quadrature Encoder Driver for ATSAM3X8E
 * 
 * Implementation of TC2 quadrature decoder for motor position sensing.
 */

#include "encoder.h"
#include "sam.h"
#include "uart.h"
#include <stdio.h>

// Track last position for direction detection and filtering
static int16_t last_position = 0;
static int16_t last_valid_position = 0;

bool encoder_init(void) {
    // 1. Enable peripheral clock for TC2 (Timer Counter 2)
    PMC->PMC_PCER1 |= (1 << (ID_TC6 - 32));
    
    // 2. Configure PIO pins for TC2 peripheral control
    // PC25 = TIOA6 (Channel A), PC26 = TIOB6 (Channel B)
    PIOC->PIO_PDR |= PIO_PC25 | PIO_PC26;
    PIOC->PIO_ABSR |= PIO_PC25 | PIO_PC26;
    PIOC->PIO_PUER |= PIO_PC25 | PIO_PC26;
    
    // 3. Configure TC2 Channel 0 (TC6) for quadrature decoder mode
    TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;
    TC2->TC_CHANNEL[0].TC_CMR = TC_CMR_TCCLKS_XC0;
    
    TC2->TC_BMR = TC_BMR_QDEN |        // Enable quadrature decoder
                  TC_BMR_POSEN |       // Enable position measurement
                  TC_BMR_SPEEDEN;      // Enable speed mode for stability
    
    // 4. Reset counter and enable clock
    TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
    
    return true;
}

int16_t encoder_read(void) {
    // Read CV (Counter Value) register
    // In quadrature mode, this contains the position count
    int16_t raw_position = (int16_t)TC2->TC_CHANNEL[0].TC_CV;
    
    // Software filtering: reject HUGE impossible jumps (likely overflow/glitch)
    // With full motor speed, travel is ~5000 counts in 5 seconds = 1000 counts/sec
    // At 10kHz loop (0.1ms), max ~0.1 counts/read in normal operation
    // But during fast movements or at startup, allow up to 1000 counts
    // This mainly protects against 16-bit counter overflow wraparound
    int16_t delta = raw_position - last_valid_position;
    
    if (last_valid_position != 0) {  // Skip check on first read
        // Only reject truly impossible jumps (counter overflow)
        if (abs(delta) > 10000) {
            // Massive jump - likely 16-bit overflow or glitch
            return last_valid_position;
        }
    }
    
    // Valid reading, update last valid position
    last_valid_position = raw_position;
    return raw_position;
}

void encoder_reset(void) {
    // Software trigger resets counter to 0
    TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_SWTRG;
    last_position = 0;
    last_valid_position = 0;  // Also reset filter
    printf("Encoder position reset to 0\n");
}

float encoder_get_revolutions(void) {
    int16_t position = encoder_read();
    return (float)position / ENCODER_PPR;
}

bool encoder_get_direction(void) {
    int16_t current_position = encoder_read();
    bool direction = (current_position >= last_position);
    last_position = current_position;
    return direction;
}

void encoder_print_status(void) {
    int16_t position = encoder_read();
    float revolutions = encoder_get_revolutions();
    bool forward = encoder_get_direction();
    
    printf("Encoder: Pos=%d | Rev=%.3f | Dir=%s\n", 
           position, 
           revolutions,
           forward ? "FWD" : "REV");
}
