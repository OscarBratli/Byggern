#include "pin.h"

void pin_digital_setup(int pin, enum PinMode mode)
{
    if (mode == PIN_MODE_OUTPUT)
    {
        printf("Setting pin %d as OUTPUT\n", pin);
    }
    else if (mode == PIN_MODE_INPUT)
    {
        printf("Setting pin %d as INPUT\n", pin);
    }
}

bool pin_digital_write(int pin, bool value)
{
    printf("Writing value %d to pin %d\n", value, pin);
    return true;
}

bool pin_digital_read(int pin)
{
    printf("Reading value from pin %d\n", pin);
    return true;
}

void pin_analog_setup(int pin, enum PinMode mode)
{
    if (mode == PIN_MODE_OUTPUT)
    {
        printf("Setting pin %d as OUTPUT\n", pin);
    }
    else if (mode == PIN_MODE_INPUT)
    {
        adc_init();
        printf("Setting pin %d as INPUT\n", pin);
    }
}

bool pin_analog_write(int pin, int value)
{
    printf("Writing value %d to pin %d\n", value, pin);
    return true;
}

int pin_analog_read(int pin)
{
    return adc_read(pin);
}
