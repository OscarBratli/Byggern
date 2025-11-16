#include "solenoid.h"
#include "sam.h"

// PB25 is used for solenoid control (digital pin 2 on Arduino Due)
// Connected through PNP transistor driver to relay
// PNP logic: LOW = ON (solenoid active), HIGH = OFF (solenoid inactive)

#define SOLENOID_PIN_MASK (1 << 25)
#define SOLENOID_DEFAULT_PULSE_MS 50  // Default pulse duration in milliseconds

void solenoid_init(void) {
    // Enable clock for PIOB (Parallel I/O Controller B)
    PMC->PMC_PCER0 = (1 << ID_PIOB);
    
    // Enable PIO control on PB25
    PIOB->PIO_PER = SOLENOID_PIN_MASK;
    
    // Configure PB25 as output
    PIOB->PIO_OER = SOLENOID_PIN_MASK;
    
    // Initialize to HIGH (solenoid OFF due to PNP logic)
    PIOB->PIO_SODR = SOLENOID_PIN_MASK;
}

void solenoid_set(uint8_t active) {
    if (active) {
        // Set LOW to activate solenoid (PNP transistor ON)
        PIOB->PIO_CODR = SOLENOID_PIN_MASK;
    } else {
        // Set HIGH to deactivate solenoid (PNP transistor OFF)
        PIOB->PIO_SODR = SOLENOID_PIN_MASK;
    }
}

void solenoid_fire(uint16_t duration_ms) {
    // Use default pulse duration if 0 is passed
    if (duration_ms == 0) {
        duration_ms = SOLENOID_DEFAULT_PULSE_MS;
    }
    
    // Activate solenoid
    solenoid_set(1);
    
    // Simple delay loop (very approximate timing)
    // For more accurate timing, consider using a hardware timer
    // Assuming 84 MHz CPU clock, approximate delay
    volatile uint32_t delay_cycles = duration_ms * 84000UL / 4;  // Rough approximation
    for (volatile uint32_t i = 0; i < delay_cycles; i++) {
        __asm__("nop");
    }
    // Deactivate solenoid
    solenoid_set(0);
}
