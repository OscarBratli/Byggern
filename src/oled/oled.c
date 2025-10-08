#include "oled.h"
#include "spi.h"
#include <util/delay.h>
#include "fonts.h"  // optional: font8[96][8] from lab file
#include <avr/pgmspace.h>

void oled_init(void)
{
    spi_init();
    oled_reset_pulse();

    oled_set_command_mode();
    spi_select();

    // === SSD1309 initialization sequence ===
    spi_transfer(0xAE); // Display OFF
    spi_transfer(0xA1); // Segment remap
    spi_transfer(0xC8); // COM scan direction
    spi_transfer(0xA8); spi_transfer(0x3F); // Multiplex ratio 1/64
    spi_transfer(0xD3); spi_transfer(0x00); // Display offset = 0
    spi_transfer(0xD5); spi_transfer(0x80); // Clock divide
    spi_transfer(0x81); spi_transfer(0x7F); // Contrast
    spi_transfer(0xD9); spi_transfer(0xF1); // Precharge period
    spi_transfer(0xDA); spi_transfer(0x12); // COM pins config
    spi_transfer(0xDB); spi_transfer(0x40); // VCOMH deselect
    spi_transfer(0x20); spi_transfer(0x00); // Horizontal addressing
    spi_transfer(0x8D); spi_transfer(0x14); // Enable charge pump
    spi_transfer(0xA4); // Resume from RAM
    spi_transfer(0xA6); // Normal (not inverted)
    spi_transfer(0xAF); // Display ON

    spi_deselect();
    _delay_ms(50);

    oled_clear();
    oled_home();
}

void oled_clear(void)
{
    for (uint8_t page = 0; page < 8; page++) {
        oled_goto_line(page);
        oled_goto_column(0);
        oled_set_data_mode();
        spi_select();
        for (uint8_t col = 0; col < 128; col++)
            spi_transfer(0x00);
        spi_deselect();
    }
}

void oled_home(void)
{
    oled_set_cursor(0, 0);
}

void oled_goto_line(uint8_t line)
{
    oled_set_command_mode();
    spi_select();
    spi_transfer(0xB0 | (line & 0x0F));  // Set page address
    spi_deselect();
}

void oled_goto_column(uint8_t col)
{
    oled_set_command_mode();
    spi_select();
    spi_transfer(0x00 | (col & 0x0F));         // Lower nibble
    spi_transfer(0x10 | ((col >> 4) & 0x0F));  // Upper nibble
    spi_deselect();
}

void oled_set_cursor(uint8_t line, uint8_t col)
{
    oled_goto_line(line);
    oled_goto_column(col);
}

void oled_write_data(uint8_t data)
{
    oled_set_data_mode();
    spi_select();
    spi_transfer(data);
    spi_deselect();
}

void oled_write_char(char c)
{
    if (c < 32 || c > 127) return;
    oled_set_data_mode();
    spi_select();
    for (uint8_t i = 0; i < 8; i++)
        spi_transfer(pgm_read_byte(&font8[c - 32][i]));
    spi_deselect();
}

void oled_print(const char *str)
{
    while (*str)
        oled_write_char(*str++);
}

void oled_set_brightness(uint8_t level)
{
    oled_set_command_mode();
    spi_select();
    spi_transfer(0x81);      // Contrast control
    spi_transfer(level);     // 0–255
    spi_deselect();
}
