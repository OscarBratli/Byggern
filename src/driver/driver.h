#pragma once

/**
 * Joystick and Slider Driver
 * 
 * This driver provides functions to read joystick and slider positions from ADC.
 * 
 * HARDWARE CONFIGURATION:
 * - ADC Channel 0: Joystick Y axis
 * - ADC Channel 1: Joystick X axis
 * - ADC Channel 2: Slider/Touchpad X axis
 * - ADC Channel 3: Slider/Touchpad Y axis
 * 
 * MEASURED ADC VALUES (from hardware testing):
 * 
 * Joystick Y (Channel 0):
 *   - Center: ADC = 160, Voltage = 2.472V
 *   - Maximum: ADC = 240, Voltage = 3.857V
 *   - Minimum: ADC = 80, Voltage = 1.165V
 * 
 * Joystick X (Channel 1):
 *   - Center: ADC = 160 ± 1, Voltage = 2.505V
 *   - Maximum: ADC = 251, Voltage = 4.08V
 *   - Minimum: ADC = 61, Voltage = 0.934V
 * 
 * Slider X (Channel 2):
 *   - Center: ADC = 159, Voltage = 2.505V
 *   - Maximum: ADC = 255, Voltage = 4.967V
 *   - Minimum: ADC = 7, Voltage = 0.128V
 * 
 * Slider Y (Channel 3):
 *   - Center: ADC = 181, Voltage = 2.85V
 *   - Maximum: ADC = (not provided, estimated)
 *   - Minimum: ADC = (not provided, estimated)
 * 
 * VOLTAGE TO ADC RELATIONSHIP:
 * The relationship between voltage and ADC value is approximately linear:
 *   ADC = (Voltage - V_min) / (V_max - V_min) * (ADC_max - ADC_min) + ADC_min
 * 
 * For Joystick X: ADC ≈ (V - 0.934) / (4.08 - 0.934) * 190 + 61
 * For Joystick Y: ADC ≈ (V - 1.165) / (3.857 - 1.165) * 160 + 80
 * For Slider X: ADC ≈ (V - 0.128) / (4.967 - 0.128) * 248 + 7
 */

#include <stdint.h>
#include "joystick/joystick.h"
#include "slider/slider.h"
#include "adc/adc.h"
#include "vec2/vec2.h"

// ADC Channel assignments
#define JOYSTICK_Y_CHANNEL 0
#define JOYSTICK_X_CHANNEL 1
#define SLIDER_X_CHANNEL 2
#define SLIDER_Y_CHANNEL 3

// Joystick ADC calibration values (measured from hardware)
#define JOYSTICK_X_MIN 61
#define JOYSTICK_X_CENTER 160
#define JOYSTICK_X_MAX 251

#define JOYSTICK_Y_MIN 80
#define JOYSTICK_Y_CENTER 160
#define JOYSTICK_Y_MAX 240

// Slider ADC calibration values (measured from hardware)
#define SLIDER_X_MIN 7
#define SLIDER_X_CENTER 159
#define SLIDER_X_MAX 255

#define SLIDER_Y_MIN 0      // Estimated, not provided in measurements
#define SLIDER_Y_CENTER 181
#define SLIDER_Y_MAX 255    // Estimated, not provided in measurements

/**
 * Initializes the joystick and slider driver.
 * This function must be called before using any other driver functions.
 */
void driver_init(void);

/**
 * Gets the joystick as an AdcJoystick structure.
 * The joystick can then be updated and read using joystick functions.
 * 
 * @return Pointer to the AdcJoystick structure
 */
AdcJoystick* driver_get_joystick(void);

/**
 * Gets the slider as a Slider structure.
 * The slider can then be updated and read using slider functions.
 * 
 * @return Pointer to the Slider structure
 */
Slider* driver_get_slider(void);

/**
 * Reads and returns the current joystick position.
 * Position is in range [-1.0, 1.0] for both X and Y axes.
 * 
 * @return Vec2 containing the joystick position
 */
Vec2 driver_read_joystick(void);

/**
 * Reads and returns the current slider position.
 * Position is in range [0.0, 1.0] for both X and Y axes.
 * 
 * @return Vec2 containing the slider position
 */
Vec2 driver_read_slider(void);

/**
 * Reads raw ADC values for all channels.
 * 
 * @param joystick_x Output parameter for Joystick X ADC value
 * @param joystick_y Output parameter for Joystick Y ADC value
 * @param slider_x Output parameter for Slider X ADC value
 * @param slider_y Output parameter for Slider Y ADC value
 */
void driver_read_raw_adc(uint8_t *joystick_x, uint8_t *joystick_y, 
                         uint8_t *slider_x, uint8_t *slider_y);
