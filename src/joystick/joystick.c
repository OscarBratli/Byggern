#include "joystick.h"

Joystick joystick_create(Vec2 deadzone)
{
    Joystick j;
    j.deadzone = deadzone;
    return j;
}

PinJoystick joystick_pin_create(int x_pin, int y_pin, int scale_min, int scale_max, Vec2 deadzone)
{
    PinJoystick j;
    j.x_pin = x_pin;
    j.y_pin = y_pin;
    j.scale_min = scale_min;
    j.scale_max = scale_max;
    j.joystick.deadzone = deadzone;
    return j;
}

MemoryJoystick joystick_memory_create(int *x_reg, int *y_reg, int scale_min, int scale_max, Vec2 deadzone)
{
    MemoryJoystick j;
    j.x_reg = x_reg;
    j.y_reg = y_reg;
    j.scale_min = scale_min;
    j.scale_max = scale_max;
    j.joystick.deadzone = deadzone;
    return j;
}

void joystick_update(Joystick *j, float value_x, float value_y)
{
    j->raw_position.x = value_x;
    j->raw_position.y = value_y;

    // Apply deadzone
    if (value_x > j->deadzone.x || value_x < -j->deadzone.x)
    {
        // Compensate for the deadzone gap
        if (value_x > 0)
        {
            j->position.x = scale(value_x, j->deadzone.x, 1.0, 0.0, 1.0);
        }
        else
        {
            j->position.x = scale(value_x, -1.0, -j->deadzone.x, -1.0, 0.0);
        }
    }
    else
    {
        j->position.x = 0;
    }

    if (value_y > j->deadzone.y || value_y < -j->deadzone.y)
    {
        // Compensate for the deadzone gap
        if (value_y > 0)
        {
            j->position.y = scale(value_y, j->deadzone.y, 1.0, 0.0, 1.0);
        }
        else
        {
            j->position.y = scale(value_y, -1.0, -j->deadzone.y, -1.0, 0.0);
        }
    }
    else
    {
        j->position.y = 0;
    }
}

bool joystick_calibrate(Joystick *j, long duration, float margin)
{
    if (!j->calibrating)
    {
        j->calibrating = true;
        j->calibration_start_time = cpu_time_milliseconds();
        j->deadzone.x = 0.0;
        j->deadzone.y = 0.0;
    }
    else
    {

        if (j->raw_position.x > j->deadzone.x)
        {
            j->deadzone.x = abs(j->raw_position.x);
        }
        if (j->raw_position.y > j->deadzone.y)
        {
            j->deadzone.y = abs(j->raw_position.y);
        }

        if (cpu_time_milliseconds() - j->calibration_start_time >= duration)
        {
            j->deadzone.x *= margin;
            j->deadzone.y *= margin;
            j->calibrating = false;
        }
    }

    return j->calibrating;
}

void joystick_memory_calibrate_blocking(MemoryJoystick *j, long duration, float margin)
{
    while (joystick_calibrate(&j->joystick, duration, margin))
    {
        joystick_memory_update(j);
    }
}

void joystick_pin_calibrate_blocking(PinJoystick *j, long duration, float margin)
{
    while (joystick_calibrate(&j->joystick, duration, margin))
    {
        joystick_pin_update(j);
    }
}

void joystick_pin_setup(PinJoystick *j)
{
    pin_analog_setup(j->x_pin, PIN_MODE_INPUT);
    pin_analog_setup(j->y_pin, PIN_MODE_INPUT);
}

void joystick_pin_update(PinJoystick *j)
{

    joystick_update(
        &j->joystick,
        scale(pin_analog_read(j->x_pin), j->scale_min, j->scale_max, -1.0, 1.0),
        scale(pin_analog_read(j->y_pin), j->scale_min, j->scale_max, -1.0, 1.0));
}

void joystick_memory_update(MemoryJoystick *j)
{
    joystick_update(
        &j->joystick,
        scale(*(j->x_reg), j->scale_min, j->scale_max, -1.0, 1.0),
        scale(*(j->y_reg), j->scale_min, j->scale_max, -1.0, 1.0));
}

Vec2 joystick_get_position_centered(Joystick *j)
{
    return j->position;
}

Vec2 joystick_get_position_centered_x(Joystick *j)
{
    Vec2 pos;
    pos.x = j->position.x;
    pos.y = scale(j->position.y, -1.0, 1.0, 0.0, 1.0);
    return pos;
}

Vec2 joystick_get_position_scaled(Joystick *j, float min_x, float max_x, float min_y, float max_y)
{
    Vec2 scaled_position;
    scaled_position.x = scale(j->position.x, -1.0, 1.0, min_x, max_x);
    scaled_position.y = scale(j->position.y, -1.0, 1.0, min_y, max_y);
    return scaled_position;
}
