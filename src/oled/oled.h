#ifndef OLED_H
#define OLED_H

#include <avr/io.h>
#include <stdint.h>

// OLED Display dimensions
#define OLED_WIDTH  128
#define OLED_HEIGHT 64
#define OLED_PAGES  8   // 64 pixels / 8 = 8 pages

// OLED Control pins (from spi.h)
#define OLED_DC   PB2   // Data/Command pin
#define OLED_CS   PB3   // Chip Select pin  
#define OLED_RES  PD5   // Reset pin

// SSD1306 Commands ()
#define OLED_DISPLAYOFF             0xAE
#define OLED_DISPLAYON              0xAF
#define OLED_CHARGEPUMP             0x8D
#define OLED_SEGREMAP               0xA1
#define OLED_COMSCANDEC             0xC8
#define OLED_SETCONTRAST            0x81
#define OLED_DISPLAYALLON_RESUME    0xA4
#define OLED_NORMALDISPLAY          0xA6
#define OLED_MEMORYMODE             0x20

// Page Mode Commands (lab manual recommendation - easiest to use)
#define OLED_PAGE_MODE              0x10    // Page Addressing Mode (default)
#define OLED_SET_PAGE_ADDR          0xB0    // Set page address (0xB0-0xB7)
#define OLED_SET_COL_LOW            0x00    // Set lower column address (0x00-0x0F)
#define OLED_SET_COL_HIGH           0x10    // Set higher column address (0x10-0x1F)

// Function declarations
void oled_init(void);
void oled_write_command(uint8_t cmd);
void oled_write_data(uint8_t data);
void oled_fill_screen_white(void);
void oled_clear_screen(void);
void oled_print_char(char c, uint8_t x, uint8_t y);
void oled_print_string(char* str, uint8_t x, uint8_t y);
void oled_set_cursor_page_mode(uint8_t page, uint8_t column);

#endif
