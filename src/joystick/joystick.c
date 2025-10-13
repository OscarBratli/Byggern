#include "joystick.h"
#include "adc/adc.h"
#include <stdbool.h>

// Auto-calibration variables
static struct {
    uint16_t x_min, x_max;
    uint16_t y_min, y_max;
    bool initialized;
} joystick_cal = {0};

// Auto-calibration functions
static void joystick_auto_calibrate(uint16_t adc_x, uint16_t adc_y)
{
    if (!joystick_cal.initialized) {
        // First reading - initialize with current values
        joystick_cal.x_min = joystick_cal.x_max = adc_x;
        joystick_cal.y_min = joystick_cal.y_max = adc_y;
        joystick_cal.initialized = true;
    } else {
        // Update min/max values
        if (adc_x < joystick_cal.x_min) joystick_cal.x_min = adc_x;
        if (adc_x > joystick_cal.x_max) joystick_cal.x_max = adc_x;
        if (adc_y < joystick_cal.y_min) joystick_cal.y_min = adc_y;
        if (adc_y > joystick_cal.y_max) joystick_cal.y_max = adc_y; 
    }
}

// Helper functions
static uint8_t normalize_to_percentage(uint16_t adc_value, uint16_t min_val, uint16_t max_val)
{
    // Clamp to valid range
    if (adc_value <= min_val) return 0;
    if (adc_value >= max_val) return 100;
    
    // Linear scaling from [min_val, max_val] to [0, 100]
    uint32_t result = ((uint32_t)(adc_value - min_val) * 100) / (max_val - min_val);
    
    return (uint8_t)result;
}

static uint8_t normalize_to_byte(uint16_t adc_value, uint16_t min_val, uint16_t max_val)
{
    // Clamp to valid range
    if (adc_value <= min_val) return 0;
    if (adc_value >= max_val) return 255;
    
    // Linear scaling from [min_val, max_val] to [0, 255]
    uint32_t result = ((uint32_t)(adc_value - min_val) * 255) / (max_val - min_val);
    
    return (uint8_t)result;
}

joystick_pos_t joystick_get_position(void)
{
    joystick_pos_t pos;
    
    // Read ADC values
    uint16_t adc_x = adc_read(JOYSTICK_ADC_X_CHANNEL);  // A1
    uint16_t adc_y = adc_read(JOYSTICK_ADC_Y_CHANNEL);  // A0
    
    // Auto-calibrate based on current readings
    joystick_auto_calibrate(adc_x, adc_y);
    
    // Use calibrated values for normalization, with fallback to defaults
    uint16_t x_min = joystick_cal.initialized ? joystick_cal.x_min : JOYSTICK_ADC_X_MIN;
    uint16_t x_max = joystick_cal.initialized ? joystick_cal.x_max : JOYSTICK_ADC_X_MAX;
    uint16_t y_min = joystick_cal.initialized ? joystick_cal.y_min : JOYSTICK_ADC_Y_MIN;
    uint16_t y_max = joystick_cal.initialized ? joystick_cal.y_max : JOYSTICK_ADC_Y_MAX;
    
    // Normalize to 0 to 100% range using calibrated values
    pos.x = normalize_to_percentage(adc_x, x_min, x_max);
    pos.y = normalize_to_percentage(adc_y, y_min, y_max);
    
    // TODO: Add button reading if needed
    pos.button = 0;
    
    return pos;
}

slider_pos_t slider_get_position(void)
{
    slider_pos_t pos;
    
    // Read ADC values
    uint16_t adc_x = adc_read(SLIDER_ADC_X_CHANNEL);  // A2
    uint16_t adc_y = adc_read(SLIDER_ADC_Y_CHANNEL);  // A3
    
    // Normalize to 0-255 range
    pos.x = normalize_to_byte(adc_x, SLIDER_ADC_X_MIN, SLIDER_ADC_X_MAX);
    pos.y = normalize_to_byte(adc_y, SLIDER_ADC_Y_MIN, SLIDER_ADC_Y_MAX);
    
    return pos;
}

// Calibration management functions
void joystick_reset_calibration(void)
{
    joystick_cal.initialized = false;
}

void joystick_get_calibration(uint16_t *x_min, uint16_t *x_max, uint16_t *y_min, uint16_t *y_max)
{
    if (joystick_cal.initialized) {
        *x_min = joystick_cal.x_min;
        *x_max = joystick_cal.x_max;
        *y_min = joystick_cal.y_min;
        *y_max = joystick_cal.y_max;
    } else {
        // Return default values if not calibrated yet
        *x_min = JOYSTICK_ADC_X_MIN;
        *x_max = JOYSTICK_ADC_X_MAX;
        *y_min = JOYSTICK_ADC_Y_MIN;
        *y_max = JOYSTICK_ADC_Y_MAX;
    }
}

void display_joystick(void) {

    // Get joystick and slider positions
    joystick_pos_t joy_pos = joystick_get_position();
    slider_pos_t slider_pos = slider_get_position();
    
    // Create strings for OLED display
    char joy_str[20], slider_str[20];
    snprintf(joy_str, sizeof(joy_str), "Joy: %u%%, %u%%", joy_pos.x, joy_pos.y);
    snprintf(slider_str, sizeof(slider_str), "Slide: %u, %u", slider_pos.x, slider_pos.y);
    
    // Display on OLED
    oled_clear_screen();
    oled_print_string("Input Test", 0, 0);
    oled_print_string(joy_str, 0, 2);
    oled_print_string(slider_str, 0, 3);
    
    // Also send to serial for debugging
    printf("Joystick: X=%u%%, Y=%u%% | Slider: X=%u, Y=%u\r\n", 
           joy_pos.x, joy_pos.y, slider_pos.x, slider_pos.y);
    
    _delay_ms(100);  // Update every 100ms
    
}
