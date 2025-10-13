#include "driver.h"
#include <stdbool.h>

// Static instances of joystick and slider
static AdcJoystick joystick;
static Slider slider;
static bool initialized = false;

void driver_init(void)
{
    if (initialized)
    {
        return;
    }
    
    // Initialize ADC
    adc_init();
    
    // Create joystick with measured calibration values
    // Joystick X is on channel 1, Y is on channel 0
    joystick = joystick_adc_create(
        JOYSTICK_X_CHANNEL,
        JOYSTICK_Y_CHANNEL,
        JOYSTICK_X_MIN,
        JOYSTICK_X_MAX,
        JOYSTICK_Y_MIN,
        JOYSTICK_Y_MAX,
        (Vec2){0.0, 0.0},  // Rest position at center
        (Vec2){0.05, 0.05} // Small deadzone
    );
    
    // Create slider with measured calibration values
    slider = slider_create(
        SLIDER_X_MIN,
        SLIDER_X_MAX,
        SLIDER_Y_MIN,
        SLIDER_Y_MAX
    );
    
    initialized = true;
}

AdcJoystick* driver_get_joystick(void)
{
    return &joystick;
}

Slider* driver_get_slider(void)
{
    return &slider;
}

Vec2 driver_read_joystick(void)
{
    joystick_adc_update(&joystick);
    return joystick_get_position_centered(&joystick.joystick);
}

Vec2 driver_read_slider(void)
{
    uint8_t adc_x = adc_read(SLIDER_X_CHANNEL);
    uint8_t adc_y = adc_read(SLIDER_Y_CHANNEL);
    
    slider_update(&slider, adc_x, adc_y);
    return slider_get_position(&slider);
}

void driver_read_raw_adc(uint8_t *joystick_x, uint8_t *joystick_y, 
                         uint8_t *slider_x, uint8_t *slider_y)
{
    *joystick_x = adc_read(JOYSTICK_X_CHANNEL);
    *joystick_y = adc_read(JOYSTICK_Y_CHANNEL);
    *slider_x = adc_read(SLIDER_X_CHANNEL);
    *slider_y = adc_read(SLIDER_Y_CHANNEL);
}
