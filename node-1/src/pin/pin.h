#pragma once
#include <avr/pgmspace.h>
#include <stdbool.h>
#include "uart/uart.h"
#include "adc/adc.h"

enum PinMode
{
    PIN_MODE_INPUT = 0,
    PIN_MODE_OUTPUT = 1
};

void pin_digital_setup(int pin, enum PinMode mode);
bool pin_digital_write(int pin, bool value);
bool pin_digital_read(int pin);

void pin_analog_setup(int pin, enum PinMode mode);
bool pin_analog_write(int pin, int value);
int pin_analog_read(int pin);
