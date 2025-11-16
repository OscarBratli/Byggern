/*
 * pwm.c - PWM Driver for Arduino Due (ATSAM3X8E)
 * 
 * This driver uses the SAM3X PWM Controller to generate precise
 * PWM signals for servo control with built-in safety features.
 */

#include "pwm.h"
#include "sam.h"
#include "uart.h"
#include <stdio.h>

// Global variables to track PWM state
static uint16_t current_pulse_width_us = PWM_SERVO_CENTER_US;
static bool pwm_initialized = false;

/**
 * @brief Clamp pulse width to safe servo range
 * 
 * SAFETY FEATURE: Prevents servo damage by ensuring pulse width
 * never goes outside the valid range (900-2100us)
 */
static uint16_t pwm_clamp_pulse_width(uint16_t pulse_width_us) {
    if (pulse_width_us < PWM_SERVO_MIN_US) {
        return PWM_SERVO_MIN_US;
    }
    if (pulse_width_us > PWM_SERVO_MAX_US) {
        return PWM_SERVO_MAX_US;
    }
    return pulse_width_us;
}

/**
 * @brief Calculate PWM duty cycle value from pulse width
 * 
 * Converts microseconds to PWM counter value based on PWM period
 */
static uint32_t pwm_calculate_duty(uint16_t pulse_width_us) {
    // PWM period register value (CPRD)
    uint32_t period = PWM->PWM_CH_NUM[PWM_SERVO_CHANNEL].PWM_CPRD;
    
    // Calculate duty cycle: (pulse_width / period) * CPRD
    // duty = (pulse_width_us * CPRD) / PWM_PERIOD_US
    uint32_t duty = ((uint32_t)pulse_width_us * period) / PWM_PERIOD_US;
    
    return duty;
}

bool pwm_init(void) {
    printf("Initializing PWM controller for servo control...\n");
    
    // 1. Enable PWM peripheral clock
    PMC->PMC_PCER1 |= (1 << (ID_PWM - 32));
    printf("- PWM clock enabled\n");
    
    // 2. Disable PWM channel during configuration
    PWM->PWM_DIS = (1 << PWM_SERVO_CHANNEL);
    
    // 3. Configure PIO for PWM output (PB13 = PWMH1, Arduino D21 = SIGNAL pin)
    // Disable PIO control and enable peripheral control
    PIOB->PIO_PDR |= PIO_PB13;    // Disable PIO
    PIOB->PIO_ABSR |= PIO_PB13;   // Select peripheral B (PWM)
    printf("- PB13 (Arduino pin D21 / SIGNAL) configured for PWM output\n");
    
    // 4. Configure PWM clock
    // Master Clock (MCK) = 84MHz
    // We want 50Hz PWM, so we need a period of 20ms
    // Using Clock A with divider to get suitable frequency
    
    // CLKA = MCK / (2^DIVA) = 84MHz / 64 = 1.3125MHz
    // This gives us a tick period of ~0.762us
    // For 20ms period: 20000us / 0.762us = 26250 ticks
    
    PWM->PWM_CLK = PWM_CLK_PREA(6)    // Divider = 2^6 = 64
                 | PWM_CLK_DIVA(1);   // Linear divider = 1 (no additional division)
    
    printf("- PWM clock configured: CLKA = 84MHz/64 = 1.3125MHz\n");
    
    // 5. Configure PWM channel
    // CPRD = Period register
    // CDTY = Duty cycle register (pulse width)
    
    // Calculate period: 20ms @ 1.3125MHz = 26250 ticks
    uint32_t period_ticks = 26250;  // For 20ms period
    
    // Set channel to use CLKA, left-aligned
    PWM->PWM_CH_NUM[PWM_SERVO_CHANNEL].PWM_CMR = 
        PWM_CMR_CPRE_CLKA |           // Use CLKA as clock source
        PWM_CMR_CPOL;                 // Start with high polarity
    
    // Set period (20ms)
    PWM->PWM_CH_NUM[PWM_SERVO_CHANNEL].PWM_CPRD = period_ticks;
    
    // Set initial duty cycle to center position (1.5ms)
    uint32_t center_duty = pwm_calculate_duty(PWM_SERVO_CENTER_US);
    PWM->PWM_CH_NUM[PWM_SERVO_CHANNEL].PWM_CDTY = center_duty;
    
    printf("- PWM period: %lu ticks (20ms)\n", period_ticks);
    printf("- Initial pulse width: %dus (center position)\n", PWM_SERVO_CENTER_US);
    
    // 6. Enable PWM channel
    PWM->PWM_ENA = (1 << PWM_SERVO_CHANNEL);
    printf("- PWM channel %d enabled\n", PWM_SERVO_CHANNEL);
    
    pwm_initialized = true;
    current_pulse_width_us = PWM_SERVO_CENTER_US;
    
    printf("✓ PWM initialization complete!\n");
    printf("  Frequency: %dHz, Period: %dms\n", PWM_FREQUENCY_HZ, PWM_PERIOD_MS);
    printf("  Safe range: %d - %d us\n", PWM_SERVO_MIN_US, PWM_SERVO_MAX_US);
    
    return true;
}

bool pwm_set_pulse_width_us(uint16_t pulse_width_us) {
    if (!pwm_initialized) {
        printf("ERROR: PWM not initialized!\n");
        return false;
    }
    
    // SAFETY: Clamp to valid servo range
    uint16_t safe_pulse_width = pwm_clamp_pulse_width(pulse_width_us);
    
    // Warn if clamping occurred
    if (safe_pulse_width != pulse_width_us) {
        printf("WARNING: Pulse width %dus clamped to safe range: %dus\n", 
               pulse_width_us, safe_pulse_width);
    }
    
    // Calculate duty cycle
    uint32_t duty = pwm_calculate_duty(safe_pulse_width);
    
    // Update duty cycle using CDTYUPD register for smooth updates
    PWM->PWM_CH_NUM[PWM_SERVO_CHANNEL].PWM_CDTYUPD = duty;
    
    current_pulse_width_us = safe_pulse_width;
    
    return true;
}

bool pwm_set_duty_percent(uint8_t duty_percent) {
    if (!pwm_initialized) {
        printf("ERROR: PWM not initialized!\n");
        return false;
    }
    
    // SAFETY: Clamp percentage to 0-100%
    if (duty_percent > 100) {
        duty_percent = 100;
    }
    
    // Map percentage to pulse width range
    // 0% = 900us, 50% = 1500us, 100% = 2100us
    uint16_t pulse_range = PWM_SERVO_MAX_US - PWM_SERVO_MIN_US;  // 1200us
    uint16_t pulse_width_us = PWM_SERVO_MIN_US + 
                              ((uint32_t)duty_percent * pulse_range) / 100;
    
    return pwm_set_pulse_width_us(pulse_width_us);
}

uint16_t pwm_get_pulse_width_us(void) {
    return current_pulse_width_us;
}

void pwm_disable(void) {
    if (!pwm_initialized) {
        return;
    }
    
    // SAFETY: Move to center position before disabling
    printf("Moving servo to center position before disabling...\n");
    pwm_set_pulse_width_us(PWM_SERVO_CENTER_US);
    
    // Small delay to let servo reach center
    for (volatile int i = 0; i < 1000000; i++);
    
    // Disable PWM channel
    PWM->PWM_DIS = (1 << PWM_SERVO_CHANNEL);
    printf("PWM disabled\n");
}

void pwm_enable(void) {
    if (!pwm_initialized) {
        return;
    }
    
    // Enable PWM channel
    PWM->PWM_ENA = (1 << PWM_SERVO_CHANNEL);
    printf("PWM enabled\n");
}

void pwm_print_status(void) {
    if (!pwm_initialized) {
        printf("PWM not initialized\n");
        return;
    }
    
    printf("\n=== PWM Status ===\n");
    printf("Channel: %d (PB16/Pin 21)\n", PWM_SERVO_CHANNEL);
    printf("Frequency: %dHz (%dms period)\n", PWM_FREQUENCY_HZ, PWM_PERIOD_MS);
    printf("Current pulse width: %dus\n", current_pulse_width_us);
    printf("Safe range: %d - %d us\n", PWM_SERVO_MIN_US, PWM_SERVO_MAX_US);
    
    // Read actual register values
    uint32_t period = PWM->PWM_CH_NUM[PWM_SERVO_CHANNEL].PWM_CPRD;
    uint32_t duty = PWM->PWM_CH_NUM[PWM_SERVO_CHANNEL].PWM_CDTY;
    
    printf("Period register (CPRD): %lu ticks\n", period);
    printf("Duty register (CDTY): %lu ticks\n", duty);
    printf("Duty percentage: %.1f%%\n", (float)duty * 100.0f / period);
    printf("==================\n\n");
}
