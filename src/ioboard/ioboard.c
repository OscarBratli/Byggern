#include "ioboard.h"

// === Private Helper Functions ===

static void ioboard_delay_us(uint16_t microseconds) {
    // Simple delay function for SPI timing requirements
    while (microseconds--) {
        _delay_us(1);
    }
}

// === Public Functions ===

void ioboard_init(void) {
    // I/O board uses the same SPI as OLED, so SPI should already be initialized
    // Just ensure SPI is in master mode
    SPI_MasterInit();
    
    // Initialize I/O board chip select pin (PB4)
    DDRB |= (1 << PB4);    // Set PB4 as output
    PORTB |= (1 << PB4);   // Set PB4 high (deselected)
}

uint8_t ioboard_spi_command(uint8_t command, uint8_t* data_buffer, uint8_t data_length) {
    // Select I/O board using IO_CS pin (PB4/SPI_SS) - NOT the OLED CS pin
    PORTB &= ~(1 << PB4);  // Pull IO_CS (PB4) low to select I/O board
    
    // Send command byte
    SPI_Transfer(command);
    
    // Wait required delay between command and first data byte (40µs minimum)
    ioboard_delay_us(IOBOARD_CMD_DATA_DELAY_US);
    
    // Read data bytes with required delays
    for (uint8_t i = 0; i < data_length; i++) {
        if (i > 0) {
            // Wait required delay between data bytes (2µs minimum)
            ioboard_delay_us(IOBOARD_DATA_DELAY_US);
        }
        data_buffer[i] = SPI_Transfer(0x00);  // Send dummy byte to read
    }
    
    // Deselect I/O board
    PORTB |= (1 << PB4);   // Pull IO_CS (PB4) high to deselect I/O board
    
    return data_length;
}

// Touchpad functions removed to save RAM

ioboard_joystick_t ioboard_read_joystick(void) {
    uint8_t data[3];
    ioboard_spi_command(IOBOARD_CMD_JOYSTICK, data, 3);
    
    ioboard_joystick_t result = {
        .x = data[0],
        .y = data[1],
        .button = data[2]
    };
    return result;
}

ioboard_buttons_t ioboard_read_buttons(void) {
    uint8_t data[3];
    ioboard_spi_command(IOBOARD_CMD_BUTTONS, data, 3);
    
    ioboard_buttons_t result = {
        .right = data[0],
        .left = data[1],
        .nav = data[2]
    };
    return result;
}

// Info function removed to save RAM

void ioboard_led_set(uint8_t led_num, bool on_off) {
    uint8_t data[2];
    data[0] = led_num;
    data[1] = on_off ? 1 : 0;
    
    // For LED commands, we send data instead of reading
    SPI_Select();
    SPI_Transfer(IOBOARD_CMD_LED_ONOFF);
    ioboard_delay_us(IOBOARD_CMD_DATA_DELAY_US);
    SPI_Transfer(data[0]);
    ioboard_delay_us(IOBOARD_DATA_DELAY_US);
    SPI_Transfer(data[1]);
    SPI_Deselect();
}

void ioboard_led_pwm(uint8_t led_num, uint8_t brightness) {
    uint8_t data[2];
    data[0] = led_num;
    data[1] = brightness;  // 0-255 for PWM width
    
    // For LED PWM commands, we send data instead of reading
    SPI_Select();
    SPI_Transfer(IOBOARD_CMD_LED_PWM);
    ioboard_delay_us(IOBOARD_CMD_DATA_DELAY_US);
    SPI_Transfer(data[0]);
    ioboard_delay_us(IOBOARD_DATA_DELAY_US);
    SPI_Transfer(data[1]);
    SPI_Deselect();
}

void btn_test(void){

   // Read I/O board button states
    ioboard_buttons_t buttons = ioboard_read_buttons();
    
    // Print button states in compact format
    printf("R%d | L%d | Nav: 0x%02X\r\n", 
           buttons.right, buttons.left, buttons.nav);
    
    // Wait before next reading
    _delay_ms(200);

}