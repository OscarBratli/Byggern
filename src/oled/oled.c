#include "oled.h"
#include "spi/spi.h"
#include "fonts/fonts.h"
#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

// Write command to OLED
void oled_write_command(uint8_t cmd) {
    PORTB &= ~(1 << OLED_CS);   // Select OLED
    PORTB &= ~(1 << OLED_DC);   // Command mode (DC low)
    SPI_MasterTransmit(cmd);
    PORTB |= (1 << OLED_CS);    // Deselect OLED
    _delay_us(1);               // Small delay for stability
}

// Write data to OLED
void oled_write_data(uint8_t data) {
    PORTB &= ~(1 << OLED_CS);   // Select OLED
    PORTB |= (1 << OLED_DC);    // Data mode (DC high)
    SPI_MasterTransmit(data);
    PORTB |= (1 << OLED_CS);    // Deselect OLED
    _delay_us(1);               // Small delay for stability
}

// Simple OLED initialization
void oled_init(void) {
    // Set control pins as outputs
    DDRB |= (1 << OLED_DC) | (1 << OLED_CS);
    DDRD |= (1 << OLED_RES);
    
    // Reset sequence
    PORTD &= ~(1 << OLED_RES);  // Reset low
    _delay_ms(10);
    PORTD |= (1 << OLED_RES);   // Reset high
    _delay_ms(10);
    
    // Minimal init commands
    oled_write_command(OLED_DISPLAYOFF);            // Display OFF
    oled_write_command(OLED_CHARGEPUMP);            // Charge pump
    oled_write_command(0x14);                       // Enable charge pump
    oled_write_command(OLED_SEGREMAP);              // Segment remap
    oled_write_command(OLED_COMSCANDEC);            // COM scan direction
    oled_write_command(OLED_SETCONTRAST);           // Set contrast
    oled_write_command(0xFF);                       // Max contrast
    oled_write_command(OLED_DISPLAYALLON_RESUME);   // Resume from RAM
    oled_write_command(OLED_NORMALDISPLAY);         // Normal display
    oled_write_command(OLED_DISPLAYON);             // Display ON
}

// Simply turn entire screen white
void oled_fill_screen_white(void) {
    // Set addressing mode to horizontal
    oled_write_command(OLED_MEMORYMODE);    // Memory addressing mode
    oled_write_command(0x00);               // Horizontal addressing
    
    // Set column range (0-127)
    oled_write_command(OLED_COLUMNADDR);    // Column address
    oled_write_command(0x00);               // Start column
    oled_write_command(0x7F);               // End column (127)
    
    // Set page range (0-7)
    oled_write_command(OLED_PAGEADDR);      // Page address
    oled_write_command(0x00);               // Start page
    oled_write_command(0x07);               // End page (7)
    
    // Fill all pixels white (1024 bytes total: 128x64/8)
    for (uint16_t i = 0; i < 1024; i++) {
        oled_write_data(0xFF);
    }
}

// Clear screen (turn black)
void oled_clear_screen(void) {
    // Set addressing mode to horizontal
    oled_write_command(OLED_MEMORYMODE);    // Memory addressing mode
    oled_write_command(0x00);               // Horizontal addressing
    
    // Set column range (0-127)
    oled_write_command(OLED_COLUMNADDR);    // Column address
    oled_write_command(0x00);               // Start column
    oled_write_command(0x7F);               // End column (127)
    
    // Set page range (0-7)
    oled_write_command(OLED_PAGEADDR);      // Page address
    oled_write_command(0x00);               // Start page
    oled_write_command(0x07);               // End page (7)
    
    // Fill all pixels black (1024 bytes total: 128x64/8)
    for (uint16_t i = 0; i < 1024; i++) {
        oled_write_data(0x00);
    }
}

// Print a single character at position (x, y)
void oled_print_char(char c, uint8_t x, uint8_t y) {
    if (c < 32 || c > 126) return; // Only printable ASCII
    
    uint8_t char_index = c - 32; // Font array starts at space (ASCII 32)
    
    // Set position
    oled_write_command(OLED_COLUMNADDR);    // Column address
    oled_write_command(x);                  // Start column
    oled_write_command(x + 7);              // End column (8 pixels wide)
    
    oled_write_command(OLED_PAGEADDR);      // Page address
    oled_write_command(y);                  // Start page
    oled_write_command(y);                  // End page (1 page high)
    
    // Send character bitmap
    for (uint8_t i = 0; i < 8; i++) {
        uint8_t column = pgm_read_byte(&font8[char_index][i]);
        oled_write_data(column);
    }
}

// Print a string at position (x, y)
void oled_print_string(char* str, uint8_t x, uint8_t y) {
    uint8_t pos_x = x;
    
    while (*str && pos_x < 120) { // Don't go off screen
        oled_print_char(*str, pos_x, y);
        pos_x += 8; // Move to next character position
        str++;
    }
}


