#include <util/delay.h>
#include <stdio.h>
#include "cpu_time/cpu_time.h"
#include "spi/spi.h"
#include "oled/oled.h"
#include "driver/driver.h"
#include "uart/uart.h"
#include "utils/utils.h"

void setup(void)
{
    // Initialize UART for debugging
    uart_init(MYUBRR);
    
    // Initialize CPU time for calibration
    cpu_time_init();
    
    // Initialize SPI for OLED communication
    spi_setup();
    
    // Initialize OLED display
    oled_init();
    oled_clear_screen();
    
    // Initialize joystick and slider driver
    driver_init();
    
    // Display title on OLED
    oled_print_string("Joystick/Slider", 8, 0);
    
    printf("Joystick and Slider Driver Initialized\r\n");
}

void loop(void)
{
    // Read raw ADC values
    uint8_t joy_x_adc, joy_y_adc, slider_x_adc, slider_y_adc;
    driver_read_raw_adc(&joy_x_adc, &joy_y_adc, &slider_x_adc, &slider_y_adc);
    
    // Read processed positions
    Vec2 joy_pos = driver_read_joystick();
    Vec2 slider_pos = driver_read_slider();
    
    // Display on OLED
    char buffer[32];
    
    // Display joystick values
    snprintf(buffer, sizeof(buffer), "Joy X:%3d Y:%3d", joy_x_adc, joy_y_adc);
    oled_print_string(buffer, 0, 2);
    
    snprintf(buffer, sizeof(buffer), "J X:%+.2f Y:%+.2f", joy_pos.x, joy_pos.y);
    oled_print_string(buffer, 0, 3);
    
    // Display slider values
    snprintf(buffer, sizeof(buffer), "Sld X:%3d Y:%3d", slider_x_adc, slider_y_adc);
    oled_print_string(buffer, 0, 5);
    
    snprintf(buffer, sizeof(buffer), "S X:%.2f Y:%.2f", slider_pos.x, slider_pos.y);
    oled_print_string(buffer, 0, 6);
    
    // Also print to UART for debugging
    printf("Joy ADC: X=%3d Y=%3d | Pos: X=%+.2f Y=%+.2f | ",
           joy_x_adc, joy_y_adc, joy_pos.x, joy_pos.y);
    printf("Slider ADC: X=%3d Y=%3d | Pos: X=%.2f Y=%.2f\r\n",
           slider_x_adc, slider_y_adc, slider_pos.x, slider_pos.y);
    
    _delay_ms(100);
}

int main(void)
{
    setup();
    
    while (1)
    {
        loop();
    }
    
    return 0;
}
