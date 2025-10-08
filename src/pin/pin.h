#pragma once
#include <stdio.h>
#include <stdbool.h>
#include "adc/adc.h"

enum PinMode
{
    PIN_MODE_INPUT = 0,
    PIN_MODE_OUTPUT = 1
};

void pin_digital_setup(int pin, enum PinMode mode);
bool pin_digital_write(int pin, bool value);
bool pin_digital_read(int pin);
