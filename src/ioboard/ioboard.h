#ifndef IOBOARD_H
#define IOBOARD_H

#include <stdint.h>
#include <stdbool.h>

// I/O Board SPI Commands (from the documentation image)
#define IOBOARD_CMD_TOUCHPAD    0x01    // Read touchpad (X, Y, size)
#define IOBOARD_CMD_TOUCHSLIDER 0x02    // Read touch slider (X, size)  
#define IOBOARD_CMD_JOYSTICK    0x03    // Read joystick (X, Y, btn)
#define IOBOARD_CMD_BUTTONS     0x04    // Read buttons (right, left, nav)
#define IOBOARD_CMD_LED_ONOFF   0x05    // LED on/off control
#define IOBOARD_CMD_LED_PWM     0x06    // LED PWM control
#define IOBOARD_CMD_INFO        0x07    // Read board info

// SPI timing constraints (from documentation)
#define IOBOARD_CMD_DATA_DELAY_US   40  // 40µs minimum between command and first data
#define IOBOARD_DATA_DELAY_US       2   // 2µs minimum between data bytes

// Data structures for I/O board responses
// NOTE: Joystick center button (JOY_B) is directly connected to ATmega162 PB1
// All other buttons/inputs come through SPI communication below

typedef struct {
    uint8_t x;
    uint8_t y; 
    uint8_t size;  // Signal strength/pressure
} ioboard_touchpad_t;

typedef struct {
    uint8_t x;
    uint8_t size;
} ioboard_touchslider_t;

typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t button;
} ioboard_joystick_t;

typedef struct {
    uint8_t right;
    uint8_t left;
    uint8_t nav;  // Navigation bits: [0 0 Btn Right Left Down Up]
} ioboard_buttons_t;

// Info struct removed to save RAM - use individual reads if needed

// === I/O Board Functions ===

// Initialize I/O board communication
void ioboard_init(void);

// Read functions - minimal set
ioboard_buttons_t ioboard_read_buttons(void);
ioboard_joystick_t ioboard_read_joystick(void);

// LED control functions
void ioboard_led_set(uint8_t led_num, bool on_off);
void ioboard_led_pwm(uint8_t led_num, uint8_t brightness);


// Low-level SPI communication
uint8_t ioboard_spi_command(uint8_t command, uint8_t* data_buffer, uint8_t data_length);


//TEST's

void btn_test(void);
#endif