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
    
    // Lab manual recommended minimal initialization:
    oled_write_command(OLED_SEGREMAP);              // A1 - Segment remap (lab manual)
    oled_write_command(OLED_COMSCANDEC);            // C8 - COM scan direction (lab manual)  
    oled_write_command(OLED_DISPLAYON);             // AF - Display ON (lab manual)
    
    // Set Page Mode (default and easiest according to lab manual)
    oled_write_command(OLED_MEMORYMODE);            // Set memory addressing mode
    oled_write_command(OLED_PAGE_MODE);             // Page addressing mode (0x10)

    oled_clear_screen();                            // Clear screen on init 
}

// Set cursor position using Page Mode (lab manual recommendation)
void oled_set_cursor_page_mode(uint8_t page, uint8_t column) {
    // Set page address (0xB0 + page number)
    oled_write_command(OLED_SET_PAGE_ADDR + (page & 0x07));
    
    // Set column address (split into low and high nibbles)
    oled_write_command(OLED_SET_COL_LOW + (column & 0x0F));       // Lower 4 bits
    oled_write_command(OLED_SET_COL_HIGH + ((column >> 4) & 0x0F)); // Upper 4 bits
}

// Fill entire screen white using Page Mode (lab manual approach)
void oled_fill_screen_white(void) {
    // Fill each page (row) one by one
    for (uint8_t page = 0; page < 8; page++) {
        oled_set_cursor_page_mode(page, 0);  // Set to start of page
        
        // Fill entire page (128 columns) with white pixels
        for (uint8_t col = 0; col < 128; col++) {
            oled_write_data(0xFF);
        }
    }
}

// Clear screen (turn black) using Page Mode (lab manual approach)
void oled_clear_screen(void) {
    // Clear each page (row) one by one
    for (uint8_t page = 0; page < 8; page++) {
        oled_set_cursor_page_mode(page, 0);  // Set to start of page
        
        // Clear entire page (128 columns) with black pixels
        for (uint8_t col = 0; col < 128; col++) {
            oled_write_data(0x00);
        }
    }
}

// Print a single character at position (x, y) using Page Mode
void oled_print_char(char c, uint8_t x, uint8_t y) {
    if (c < 32 || c > 126) return; // Only printable ASCII
    
    uint8_t char_index = c - 32; // Font array starts at space (ASCII 32)
    
    // Set position using page mode
    oled_set_cursor_page_mode(y, x);
    
    // Send character bitmap (8 pixels wide)
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


