#pragma once

#include <stdint.h>
#include "vec2/vec2.h"
#include "scale/scale.h"

/**
 * Structure to hold slider/touchpad ADC configuration and position data.
 */
typedef struct
{
    uint8_t x_min;      // Minimum ADC value for X axis
    uint8_t x_max;      // Maximum ADC value for X axis
    uint8_t y_min;      // Minimum ADC value for Y axis (if applicable)
    uint8_t y_max;      // Maximum ADC value for Y axis (if applicable)
    Vec2 position;      // Current position in range [0.0, 1.0] for both axes
} Slider;

/**
 * Creates and initializes a slider structure.
 * 
 * @param x_min Minimum ADC value for X axis
 * @param x_max Maximum ADC value for X axis
 * @param y_min Minimum ADC value for Y axis
 * @param y_max Maximum ADC value for Y axis
 * @return Initialized Slider structure
 */
Slider slider_create(uint8_t x_min, uint8_t x_max, uint8_t y_min, uint8_t y_max);

/**
 * Updates the slider position based on ADC readings.
 * 
 * @param s Pointer to the Slider structure
 * @param adc_x Raw ADC value from X channel
 * @param adc_y Raw ADC value from Y channel (optional, set to 0 if not used)
 */
void slider_update(Slider *s, uint8_t adc_x, uint8_t adc_y);

/**
 * Gets the current slider position scaled to [0.0, 1.0] range.
 * 
 * @param s Pointer to the Slider structure
 * @return Vec2 with x and y in range [0.0, 1.0]
 */
Vec2 slider_get_position(Slider *s);

/**
 * Gets the current slider position scaled to a custom range.
 * 
 * @param s Pointer to the Slider structure
 * @param min_x Minimum value for X axis
 * @param max_x Maximum value for X axis
 * @param min_y Minimum value for Y axis
 * @param max_y Maximum value for Y axis
 * @return Vec2 with x and y scaled to the specified range
 */
Vec2 slider_get_position_scaled(Slider *s, float min_x, float max_x, float min_y, float max_y);
