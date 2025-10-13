#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "pin/pin.h"
#include "vec2/vec2.h"
#include "scale/scale.h"
#include "cpu_time/cpu_time.h"
#include "adc/adc.h"

typedef struct
{
    Vec2 rest_pos;
    Vec2 deadzone;
    Vec2 position;
    Vec2 raw_position;
    bool calibrating;
    long calibration_start_time;
} Joystick;

typedef union
{
    Joystick joystick;
    struct
    {
        int x_pin;
        int y_pin;
        int scale_min;
        int scale_max;
    };
} PinJoystick;

typedef union
{
    Joystick joystick;
    struct
    {
        int *x_reg;
        int *y_reg;
        int scale_min;
        int scale_max;
    };
} MemoryJoystick;

typedef union
{
    Joystick joystick;
    struct
    {
        uint8_t x_channel;  // ADC channel for X axis
        uint8_t y_channel;  // ADC channel for Y axis
        uint8_t x_min;      // Minimum ADC value for X axis
        uint8_t x_max;      // Maximum ADC value for X axis
        uint8_t y_min;      // Minimum ADC value for Y axis
        uint8_t y_max;      // Maximum ADC value for Y axis
    };
} AdcJoystick;

/**
 * Creates a basic joystick that can be updated manually.
 * @param rest_pos The position of the joystick when at rest ([0, 0] if centered, [-1, -1] if left and down, [0, -1] if x is centered and y is down).
 * @param deadzone The deadzone for the joystick, where the joystick is considered to be at rest.
 * @return A Joystick struct representing the joystick.
 */
Joystick joystick_create(Vec2 rest_pos, Vec2 deadzone);

/**
 * Creates a joystick that reads from analog pins.
 * @param x_pin The pin number for the X axis.
 * @param y_pin The pin number for the Y axis.
 * @param scale_min The value read from the analog pin that represents the minimum joystick output.
 * @param scale_max The value read from the analog pin that represents the maximum joystick output.
 * @param rest_pos The position of the joystick when at rest ([0, 0] if centered, [-1, -1] if left and down, [0, -1] if x is centered and y is down).
 * @param deadzone The deadzone for the joystick, where the joystick is considered to be at rest.
 * @return A PinJoystick struct representing the joystick.
 */
PinJoystick joystick_pin_create(int x_pin, int y_pin, int scale_min, int scale_max, Vec2 rest_pos, Vec2 deadzone);

/**
 * Creates a joystick that reads from memory-mapped registers.
 * @param x_reg The memory address for the X axis.
 * @param y_reg The memory address for the Y axis.
 * @param scale_min The value read from the register that represents the minimum joystick output.
 * @param scale_max The value read from the register that represents the maximum joystick output.
 * @param rest_pos The position of the joystick when at rest ([0, 0] if centered, [-1, -1] if left and down, [0, -1] if x is centered and y is down).
 * @param deadzone The deadzone for the joystick, where the joystick is considered to be at rest.
 * @return A MemoryJoystick struct representing the joystick.
 */
MemoryJoystick joystick_memory_create(int *x_reg, int *y_reg, int scale_min, int scale_max, Vec2 rest_pos, Vec2 deadzone);

/**
 * Creates a joystick that reads from ADC channels.
 * Based on measured ADC values:
 * - Joystick X (channel 1): Center=160, Min=61, Max=251
 * - Joystick Y (channel 0): Center=160, Min=80, Max=240
 * 
 * Voltage to ADC relationship (approximately linear):
 * - X axis: ADC = (V - 0.934) / (4.08 - 0.934) * (251 - 61) + 61
 * - Y axis: ADC = (V - 1.165) / (3.857 - 1.165) * (240 - 80) + 80
 * 
 * @param x_channel ADC channel for X axis (typically channel 1)
 * @param y_channel ADC channel for Y axis (typically channel 0)
 * @param x_min Minimum ADC value for X axis
 * @param x_max Maximum ADC value for X axis
 * @param y_min Minimum ADC value for Y axis
 * @param y_max Maximum ADC value for Y axis
 * @param rest_pos The position of the joystick when at rest ([0, 0] if centered)
 * @param deadzone The deadzone for the joystick
 * @return An AdcJoystick struct representing the joystick
 */
AdcJoystick joystick_adc_create(uint8_t x_channel, uint8_t y_channel, 
                                uint8_t x_min, uint8_t x_max, 
                                uint8_t y_min, uint8_t y_max,
                                Vec2 rest_pos, Vec2 deadzone);

/**
 * Updates the joystick position based on raw input values, applying deadzone compensation.
 * @param j Pointer to the Joystick struct to update.
 * @param value_x The raw X axis value, typically in the range [-1.0, 1.0].
 * @param value_y The raw Y axis value, typically in the range [-1.0, 1.0].
 */
void joystick_update(Joystick *j, float value_x, float value_y);

/**
 * Calibrates the joystick by determining the average resting position over a specified duration.
 * This function helps to set the deadzone dynamically based on user input.
 * Must be called repeatedly until it returns false.
 * The update function must be called in between calls to this function to update the joystick position.
 * @param j Pointer to the Joystick struct to calibrate.
 * @param duration The duration in milliseconds over which to sample the joystick position.
 * @param margin A margin factor to apply to the maximum observed values to define the deadzone. A margin of 1.1 increases the deadzone by 10%.
 * @return true if calibration is still in progress, false if calibration is complete.
 */
bool joystick_calibrate(Joystick *j, long duration, float margin);

/**
 * Calibrates the MemoryJoystick by determining the average resting position over a specified duration.
 * This function helps to set the deadzone dynamically based on user input.
 * This function blocks until calibration is complete.
 * @param j Pointer to the MemoryJoystick struct to calibrate.
 * @param duration The duration in milliseconds over which to sample the joystick position.
 * @param margin A margin factor to apply to the maximum observed values to define the deadzone. A margin of 1.1 increases the deadzone by 10%.
 */
void joystick_memory_calibrate_blocking(MemoryJoystick *j, long duration, float margin);

/**
 * Calibrates the PinJoystick by determining the average resting position over a specified duration.
 * This function helps to set the deadzone dynamically based on user input.
 * This function blocks until calibration is complete.
 * @param j Pointer to the PinJoystick struct to calibrate.
 * @param duration The duration in milliseconds over which to sample the joystick position.
 * @param margin A margin factor to apply to the maximum observed values to define the deadzone. A margin of 1.1 increases the deadzone by 10%.
 */
void joystick_pin_calibrate_blocking(PinJoystick *j, long duration, float margin);

/**
 * Sets up the pins for a PinJoystick.
 * @param j Pointer to the PinJoystick struct to set up.
 */
void joystick_pin_setup(PinJoystick *j);

/**
 * Updates the PinJoystick by reading values from its analog pins and updating its position.
 * @param j Pointer to the PinJoystick struct to update.
 */
void joystick_pin_update(PinJoystick *j);

/**
 * Updates the MemoryJoystick by reading values from its memory-mapped registers and updating its position.
 * @param j Pointer to the MemoryJoystick struct to update.
 */
void joystick_memory_update(MemoryJoystick *j);

/**
 * Updates the AdcJoystick by reading values from its ADC channels and updating its position.
 * @param j Pointer to the AdcJoystick struct to update.
 */
void joystick_adc_update(AdcJoystick *j);

/**
 * Calibrates the AdcJoystick by determining the average resting position over a specified duration.
 * This function blocks until calibration is complete.
 * @param j Pointer to the AdcJoystick struct to calibrate.
 * @param duration The duration in milliseconds over which to sample the joystick position.
 * @param margin A margin factor to apply to the maximum observed values to define the deadzone. A margin of 1.1 increases the deadzone by 10%.
 */
void joystick_adc_calibrate_blocking(AdcJoystick *j, long duration, float margin);

/**
 * Retrieves the current joystick position with x and y in the range of [-1.0, 1.0].
 * @param j Pointer to the Joystick struct.
 * @return A Vec2 struct containing the centered X and Y positions.
 */
Vec2 joystick_get_position_centered(Joystick *j);

/**
 * Retrieves the current joystick position, with the X axis in the range of [-1.0, 1.0] and the Y axis in the range of [0.0, 1.0].
 * @param j Pointer to the Joystick struct.
 * @return A Vec2 struct containing the X position in [-1.0, 1.0] and Y position in [0.0, 1.0].
 */
Vec2 joystick_get_position_centered_x(Joystick *j);

/**
 * Retrieves the current joystick position, scaled to a specified range.
 * @param j Pointer to the Joystick struct.
 * @param min_x The minimum value for the X axis after scaling.
 * @param max_x The maximum value for the X axis after scaling.
 * @param min_y The minimum value for the Y axis after scaling.
 * @param max_y The maximum value for the Y axis after scaling.
 * @return A Vec2 struct containing the scaled X and Y positions.
 */
Vec2 joystick_get_position_scaled(Joystick *j, float min_x, float max_x, float min_y, float max_y);
