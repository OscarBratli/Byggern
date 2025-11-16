#include "joystick.h"
#include "adc/adc.h"
#include "oled/oled.h"
#include <stdbool.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

// Auto-calibration variables
static struct {
    uint16_t x_min, x_max;
    uint16_t y_min, y_max;
    bool initialized;
} joystick_cal = {0};

// Slider calibration variables
static struct {
    uint16_t x_min, x_max;
    uint16_t y_min, y_max;
    bool initialized;
} slider_cal = {0};

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

void joystick_init(void)
{
    // Configure joystick center button pin (JOY_B) as input with pull-up resistor
    // This is the only button directly connected to ATmega162 (PB1)
    // All other buttons (touchpad, slider, etc.) come via SPI from I/O board
    JOYSTICK_BUTTON_DDR &= ~(1 << JOYSTICK_BUTTON_PIN);  // Set as input
    JOYSTICK_BUTTON_PORT |= (1 << JOYSTICK_BUTTON_PIN);  // Enable pull-up
}

static uint8_t joystick_read_button(void)
{
    // Read joystick center button (JOY_B) - the only button directly connected to ATmega162
    // Button is active low (pressed = 0, released = 1) due to pull-up resistor
    // We return 1 when pressed, 0 when not pressed
    return !(JOYSTICK_BUTTON_PINREG & (1 << JOYSTICK_BUTTON_PIN));
}

joystick_pos_t joystick_get_position(void)
{
    joystick_pos_t pos;
    
    // Read ADC values
    uint16_t adc_x = adc_read(JOYSTICK_ADC_X_CHANNEL);  // A1
    uint16_t adc_y = adc_read(JOYSTICK_ADC_Y_CHANNEL);  // A0
    
    // NO auto-calibration - only use fixed defaults or explicit calibration
    // Use calibrated values if available, otherwise use fixed defaults
    uint16_t x_min = joystick_cal.initialized ? joystick_cal.x_min : JOYSTICK_ADC_X_MIN;
    uint16_t x_max = joystick_cal.initialized ? joystick_cal.x_max : JOYSTICK_ADC_X_MAX;
    uint16_t y_min = joystick_cal.initialized ? joystick_cal.y_min : JOYSTICK_ADC_Y_MIN;
    uint16_t y_max = joystick_cal.initialized ? joystick_cal.y_max : JOYSTICK_ADC_Y_MAX;
    
    // Normalize to 0 to 100% range using calibrated values
    pos.x = normalize_to_percentage(adc_x, x_min, x_max);
    pos.y = normalize_to_percentage(adc_y, y_min, y_max);
    
    // Read joystick center button (JOY_B) from PB1 (active low with pull-up)
    // Note: Other buttons (touchpad, slider) are accessed via SPI through I/O board
    pos.button = joystick_read_button();
    
    return pos;
}

slider_pos_t slider_get_position(void)
{
    slider_pos_t pos;
    
    // Read ADC values
    uint16_t adc_x = adc_read(SLIDER_ADC_X_CHANNEL);  // A2
    uint16_t adc_y = adc_read(SLIDER_ADC_Y_CHANNEL);  // A3
    
    // Use calibrated values if available, otherwise use fixed defaults
    uint16_t x_min = slider_cal.initialized ? slider_cal.x_min : SLIDER_ADC_X_MIN;
    uint16_t x_max = slider_cal.initialized ? slider_cal.x_max : SLIDER_ADC_X_MAX;
    uint16_t y_min = slider_cal.initialized ? slider_cal.y_min : SLIDER_ADC_Y_MIN;
    uint16_t y_max = slider_cal.initialized ? slider_cal.y_max : SLIDER_ADC_Y_MAX;
    
    // Normalize to 0-255 range using calibrated or default values
    pos.x = normalize_to_byte(adc_x, x_min, x_max);
    pos.y = normalize_to_byte(adc_y, y_min, y_max);
    
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

// Slider auto-calibration functions
static void slider_auto_calibrate(uint16_t adc_x, uint16_t adc_y)
{
    if (!slider_cal.initialized) {
        // First reading - initialize with current values
        slider_cal.x_min = slider_cal.x_max = adc_x;
        slider_cal.y_min = slider_cal.y_max = adc_y;
        slider_cal.initialized = true;
    } else {
        // Update min/max values
        if (adc_x < slider_cal.x_min) slider_cal.x_min = adc_x;
        if (adc_x > slider_cal.x_max) slider_cal.x_max = adc_x;
        if (adc_y < slider_cal.y_min) slider_cal.y_min = adc_y;
        if (adc_y > slider_cal.y_max) slider_cal.y_max = adc_y; 
    }
}

// Explicit calibration function - call this to calibrate joystick manually
void joystick_calibrate_now(void)
{
    // Read current ADC values and use for calibration
    uint16_t adc_x = adc_read(JOYSTICK_ADC_X_CHANNEL);  
    uint16_t adc_y = adc_read(JOYSTICK_ADC_Y_CHANNEL);  
    
    // Force calibration with current reading
    joystick_auto_calibrate(adc_x, adc_y);
}

// Explicit calibration function - call this to calibrate slider manually
void slider_calibrate_now(void)
{
    // Read current ADC values and use for calibration
    uint16_t adc_x = adc_read(SLIDER_ADC_X_CHANNEL);  
    uint16_t adc_y = adc_read(SLIDER_ADC_Y_CHANNEL);  
    
    // Force calibration with current reading
    slider_auto_calibrate(adc_x, adc_y);
}

// Reset slider calibration
void slider_reset_calibration(void)
{
    slider_cal.initialized = false;
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
    printf_P(PSTR("Joystick: X=%u%%, Y=%u%% | Slider: X=%u, Y=%u\r\n"), 
           joy_pos.x, joy_pos.y, slider_pos.x, slider_pos.y);
    
    _delay_ms(100);  // Update every 100ms
    
}
