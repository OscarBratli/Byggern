#include "oled.h"

void oled_init(void)
{
    spi_init();
    
    // Reset the OLED display
    oled_reset_pulse();
    _delay_ms(100);

    // Minimal initialization - as per TA recommendation
    oled_set_command_mode();
    oled_select();
    
    spi_transfer(0xA1);  // Segment remap (flip horizontally)
    spi_transfer(0xC8);  // COM scan direction (flip vertically)  
    spi_transfer(0xAF);  // Display ON
    
    oled_deselect();
    _delay_ms(1000);   // Longer delay after initialization

    oled_clear();
    oled_home();
}

void oled_clear(void)
{
    for (uint8_t page = 0; page < 8; page++) {
        oled_goto_line(page);
        oled_goto_column(0);
        oled_set_data_mode();
        oled_select();  // Use OLED-specific select
        for (uint8_t col = 0; col < 128; col++)
            spi_transfer(0x00);
        oled_deselect();  // Use OLED-specific deselect
    }
}

void oled_home(void)
{
    oled_set_cursor(0, 0);
}

void oled_goto_line(uint8_t line)
{
    oled_set_command_mode();
    oled_select();  // Use OLED-specific select
    spi_transfer(0xB0 | (line & 0x0F));  // Set page address
    oled_deselect();  // Use OLED-specific deselect
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
    oled_select();  // Use OLED-specific select
    for (uint8_t i = 0; i < 8; i++)
        spi_transfer(pgm_read_byte(&font8[c - 32][i]));
    oled_deselect();  // Use OLED-specific deselect
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
