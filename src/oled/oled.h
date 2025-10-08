#pragma once

#include <avr/io.h>
#include <stdint.h>
#include "spi/spi.h"
#include "utils/utils.h"
#include <util/delay.h>
#include "fonts/fonts.h"
#include <avr/pgmspace.h>

void oled_init(void);
void oled_clear(void);
void oled_home(void);
void oled_goto_line(uint8_t line);
void oled_goto_column(uint8_t col);
void oled_set_cursor(uint8_t line, uint8_t col);
void oled_write_data(uint8_t data);
void oled_write_char(char c);
void oled_print(const char *str);
void oled_set_brightness(uint8_t level);
