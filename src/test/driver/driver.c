#include "driver.h"

void driver_test_setup(void)
{
    uart_init(MYUBRR);
    driver_init();
    printf("Joystick and Slider Driver Test\r\n");
    printf("================================\r\n\r\n");
    
    printf("Hardware Configuration:\r\n");
    printf("  ADC Ch 0: Joystick Y\r\n");
    printf("  ADC Ch 1: Joystick X\r\n");
    printf("  ADC Ch 2: Slider X\r\n");
    printf("  ADC Ch 3: Slider Y\r\n\r\n");
    
    printf("Calibration Values:\r\n");
    printf("  Joystick X: Min=%d, Center=%d, Max=%d\r\n", 
           JOYSTICK_X_MIN, JOYSTICK_X_CENTER, JOYSTICK_X_MAX);
    printf("  Joystick Y: Min=%d, Center=%d, Max=%d\r\n", 
           JOYSTICK_Y_MIN, JOYSTICK_Y_CENTER, JOYSTICK_Y_MAX);
    printf("  Slider X: Min=%d, Center=%d, Max=%d\r\n", 
           SLIDER_X_MIN, SLIDER_X_CENTER, SLIDER_X_MAX);
    printf("  Slider Y: Min=%d, Center=%d, Max=%d\r\n\r\n", 
           SLIDER_Y_MIN, SLIDER_Y_CENTER, SLIDER_Y_MAX);
    
    printf("Starting measurements...\r\n\r\n");
}

void driver_test_loop(void)
{
    // Read raw ADC values
    uint8_t joy_x_adc, joy_y_adc, slider_x_adc, slider_y_adc;
    driver_read_raw_adc(&joy_x_adc, &joy_y_adc, &slider_x_adc, &slider_y_adc);
    
    // Read processed joystick position (range: -1.0 to 1.0)
    Vec2 joy_pos = driver_read_joystick();
    
    // Read processed slider position (range: 0.0 to 1.0)
    Vec2 slider_pos = driver_read_slider();
    
    // Display all values in a formatted manner
    printf("Raw ADC | Joystick X: %3d, Y: %3d | Slider X: %3d, Y: %3d\r\n",
           joy_x_adc, joy_y_adc, slider_x_adc, slider_y_adc);
    
    printf("Processed | Joystick X: %+.2f, Y: %+.2f | Slider X: %.2f, Y: %.2f\r\n\r\n",
           joy_pos.x, joy_pos.y, slider_pos.x, slider_pos.y);
    
    _delay_ms(100);
}
