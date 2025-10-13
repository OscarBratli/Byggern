#pragma once
#include <stdint.h>
#include <avr/io.h>

// Joystick center button pin assignment (JOY_B from I/O board pin 9)
#define JOYSTICK_BUTTON_PIN     PB1     // JOY_B connected to PB1 (ATmega162 Pin 2)
#define JOYSTICK_BUTTON_PORT    PORTB
#define JOYSTICK_BUTTON_DDR     DDRB
#define JOYSTICK_BUTTON_PINREG  PINB

// ADC channel assignments
#define JOYSTICK_ADC_X_CHANNEL  1       // A1 - Joystick X axis
#define JOYSTICK_ADC_Y_CHANNEL  0       // A0 - Joystick Y axis  
#define SLIDER_ADC_X_CHANNEL    2       // A2 - Slider X axis
#define SLIDER_ADC_Y_CHANNEL    3       // A3 - Slider Y axis

// Joystick calibration values (from voltage measurements)
#define JOYSTICK_ADC_X_MIN      59      // Minimum ADC value for X
#define JOYSTICK_ADC_X_MAX      250     // Maximum ADC value for X  
#define JOYSTICK_ADC_Y_MIN      74      // Minimum ADC value for Y
#define JOYSTICK_ADC_Y_MAX      238     // Maximum ADC value for Y

// Slider/Touchpad calibration values
#define SLIDER_ADC_X_MIN        0       // Minimum ADC value for slider X
#define SLIDER_ADC_X_MAX        255     // Maximum ADC value for slider X
#define SLIDER_ADC_Y_MIN        0       // Minimum ADC value for slider Y  
#define SLIDER_ADC_Y_MAX        255     // Maximum ADC value for slider Y

// Position structures
typedef struct {
    uint8_t x;          // X position (0 to 100%)
    uint8_t y;          // Y position (0 to 100%) 
    uint8_t button;     // Button state (0 = not pressed, 1 = pressed)
} joystick_pos_t;

typedef struct {
    uint8_t x;          // X position (0 to 255)
    uint8_t y;          // Y position (0 to 255)
} slider_pos_t;

/**
 * Initialize joystick (including button pin setup)
 */
void joystick_init(void);

/**
 * Gets joystick position as percentage (0-100%)
 */
joystick_pos_t joystick_get_position(void);

/**
 * Gets slider position (0-255 range)
 */
slider_pos_t slider_get_position(void);

/**
 * Reset joystick auto-calibration (forces recalibration)
 */
void joystick_reset_calibration(void);

/**
 * Get current calibration values for debugging
 */
void joystick_get_calibration(uint16_t *x_min, uint16_t *x_max, uint16_t *y_min, uint16_t *y_max);
