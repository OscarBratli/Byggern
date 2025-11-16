/**
 * @file ir_sensor.c
 * @brief IR beam break sensor driver implementation
 */

#include "ir_sensor.h"
#include "sam.h"
#include <stdio.h>

// ADC Configuration
#define IR_ADC_CHANNEL      0       // ADC Channel 0 (PA2 = Arduino A7)
#define IR_ADC_RESOLUTION   4096    // 12-bit ADC (0-4095)
#define IR_ADC_VREF_MV      3300    // 3.3V reference voltage

// Score tracking
static uint32_t goal_score = 0;

/**
 * @brief Initialize ADC for IR sensor reading
 */
void ir_sensor_init(void) {
    // Enable ADC peripheral clock
    PMC->PMC_PCER1 |= (1 << (ID_ADC - 32));
    
    // Reset ADC
    ADC->ADC_CR = ADC_CR_SWRST;
    
    // Configure ADC:
    // - PRESCAL: ADC clock = MCK / ((PRESCAL+1) * 2)
    //   With MCK=84MHz, PRESCAL=20 gives ADC_CLK = 84/(21*2) = 2MHz
    // - STARTUP: Startup time = (STARTUP+1) * 8 / ADC_CLK
    //   STARTUP=8 gives ~36μs startup
    // - SETTLING: Settling time (3 = 17 ADC clocks)
    // - TRACKTIM: Tracking time (15 = 16 ADC clocks)
    // - TRANSFER: Transfer period (1 = 2 ADC clocks)
    ADC->ADC_MR = ADC_MR_PRESCAL(20) |      // ADC clock prescaler
                  ADC_MR_STARTUP_SUT64 |     // Startup time
                  ADC_MR_SETTLING_AST3 |     // Settling time
                  ADC_MR_TRACKTIM(15) |      // Tracking time
                  ADC_MR_TRANSFER(1);        // Transfer period
    
    // Enable ADC channel 0
    ADC->ADC_CHER = (1 << IR_ADC_CHANNEL);
    
    // Configure pin PA2 as ADC input (disable PIO control, no pull-up)
    PIOA->PIO_PDR = PIO_PA2X1_AD0;   // Disable PIO control (ADC takes over)
    PIOA->PIO_PUDR = PIO_PA2X1_AD0;  // Disable pull-up
    
    printf("IR Sensor initialized on ADC Channel %d (PA2/A7)\n", IR_ADC_CHANNEL);
}

/**
 * @brief Read raw ADC value from IR photodiode
 */
uint16_t ir_sensor_read_raw(void) {
    // Start conversion
    ADC->ADC_CR = ADC_CR_START;
    
    // Wait for conversion to complete (poll DRDY flag)
    while (!(ADC->ADC_ISR & (1 << IR_ADC_CHANNEL))) {
        // Wait for channel data ready
    }
    
    // Read converted value from channel data register
    uint16_t adc_value = ADC->ADC_CDR[IR_ADC_CHANNEL] & 0xFFF;
    
    return adc_value;
}

/**
 * @brief Read IR sensor voltage in millivolts
 */
uint16_t ir_sensor_read_voltage_mv(void) {
    uint16_t adc_raw = ir_sensor_read_raw();
    
    // Convert ADC value to voltage: V = (ADC_raw * VREF) / ADC_resolution
    uint32_t voltage_mv = ((uint32_t)adc_raw * IR_ADC_VREF_MV) / IR_ADC_RESOLUTION;
    
    return (uint16_t)voltage_mv;
}

/**
 * @brief Check if IR beam is broken
 */
bool ir_sensor_is_beam_broken(uint16_t threshold_mv) {
    uint16_t voltage = ir_sensor_read_voltage_mv();
    
    // Beam is broken when voltage drops below threshold
    return (voltage < threshold_mv);
}

/**
 * @brief Calibrate sensor by averaging multiple readings
 */
uint16_t ir_sensor_calibrate(void) {
    printf("Calibrating IR sensor (ensure beam is intact)...\n");
    
    // Average 100 samples for stable baseline
    uint32_t sum = 0;
    const int num_samples = 100;
    
    for (int i = 0; i < num_samples; i++) {
        sum += ir_sensor_read_voltage_mv();
        
        // Small delay between samples (crude delay)
        for (volatile int j = 0; j < 10000; j++);
    }
    
    uint16_t baseline = sum / num_samples;
    
    printf("Calibration complete: Baseline = %u mV\n", baseline);
    printf("Suggested threshold: %u mV (70%% of baseline)\n", baseline * 7 / 10);
    
    return baseline;
}

/**
 * @brief Get current goal/score count
 */
uint32_t ir_sensor_get_score(void) {
    return goal_score;
}

/**
 * @brief Reset goal/score count to zero
 */
void ir_sensor_reset_score(void) {
    goal_score = 0;
    printf("Score reset to 0\n");
}

/**
 * @brief Increment goal/score count
 */
void ir_sensor_increment_score(void) {
    goal_score++;
    printf("GOAL! Score: %lu\n", goal_score);
}
