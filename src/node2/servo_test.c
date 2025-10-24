#include <stdint.h>
#include "sam.h"

// D21 (SCL) = PB12 = SERVO_PWM signal pin

void servo_test_init(void) {
    // Enable clock for PIOB
    PMC->PMC_PCER0 = (1 << ID_PIOB);
    
    // Configure PB12 (D21) as output
    PIOB->PIO_PER = (1 << 12);    // Enable PIO control
    PIOB->PIO_OER = (1 << 12);    // Set as output
    
    // Set pin HIGH
    PIOB->PIO_SODR = (1 << 12);
}