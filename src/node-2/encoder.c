/*
 * encoder.c - Quadrature Encoder Driver for ATSAM3X8E
 * 
 * Implementation of TC2 quadrature decoder for motor position sensing.
 */

#include "encoder.h"
#include "sam.h"
#include "uart.h"
#include <stdio.h>

// Track last position for direction detection
static int16_t last_position = 0;

bool encoder_init(void) {
    printf("\n=== Initializing Encoder (TC2 Quadrature Decoder) ===\n");
    
    // 1. Enable peripheral clock for TC2 (Timer Counter 2)
    // TC2 consists of TC6, TC7, TC8 channels
    // We use TC6 (Channel 0 of TC2) - ID is ID_TC6
    PMC->PMC_PCER1 |= (1 << (ID_TC6 - 32));
    printf("- TC6 clock enabled (ID=%d)\n", ID_TC6);
    
    // 2. Configure PIO pins for TC2 peripheral control
    // PC25 = TIOA6 (Channel A)
    // PC26 = TIOB6 (Channel B)
    
    // Disable PIO control, enable peripheral control
    PIOC->PIO_PDR |= PIO_PC25 | PIO_PC26;
    
    // Select peripheral B for TC function
    PIOC->PIO_ABSR |= PIO_PC25 | PIO_PC26;
    
    // Enable pull-ups for noise immunity
    PIOC->PIO_PUER |= PIO_PC25 | PIO_PC26;
    
    printf("- PC25 (TIOA6/Channel A) configured\n");
    printf("- PC26 (TIOB6/Channel B) configured\n");
    
    // 3. Configure TC2 Channel 0 (TC6) for quadrature decoder mode
    
    // Disable TC clock first
    TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;
    
    // Channel Mode Register configuration for quadrature decoder:
    // The SAM3X doesn't have TC_CMR_QDEC, quadrature is enabled in BMR
    // - TCCLKS: XC0 (clock from TIOA/TIOB)
    // - ETRGEDG: NONE 
    // - ABETRG: TIOA is external trigger
    // - WAVE: 0 (Capture mode)
    
    TC2->TC_CHANNEL[0].TC_CMR = 
        TC_CMR_TCCLKS_XC0 |           // Use XC0 as clock source
        TC_CMR_ETRGEDG_NONE |         // No external trigger
        TC_CMR_ABETRG;                // TIOA used as trigger
    
    // BMR (Block Mode Register) - This is where quadrature is enabled
    // QDEN: Quadrature Decoder Enabled
    // POSEN: Position Enabled (counter counts based on quadrature)
    // EDGPHA: Edges on Phase A and B
    TC2->TC_BMR = TC_BMR_QDEN |       // Quadrature decoder enabled (bit 8)
                  TC_BMR_POSEN |       // Position enabled (bit 9)
                  TC_BMR_EDGPHA;       // Edges on both phases (bit 12)
    
    printf("- Quadrature decoder mode configured\n");
    printf("- XC0 clock from TIOA/TIOB edges\n");
    
    // 4. Reset counter and enable clock
    TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
    
    printf("- Counter reset and clock enabled\n");
    printf("- Encoder PPR: %d\n", ENCODER_PPR);
    printf("=====================================\n\n");
    
    return true;
}

int16_t encoder_read(void) {
    // Read CV (Counter Value) register
    // In quadrature mode, this contains the position count
    return (int16_t)TC2->TC_CHANNEL[0].TC_CV;
}

void encoder_reset(void) {
    // Software trigger resets counter to 0
    TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_SWTRG;
    last_position = 0;
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
