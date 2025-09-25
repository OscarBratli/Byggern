#include "joystick.h"

static int *joy_x_reg = (int *)0x100; // Example memory-mapped register address for X axis
static int *joy_y_reg = (int *)0x101; // Example memory-mapped register address for Y axis

static MemoryJoystick joy;

void joystick_test_setup(void)
{
    uart_init(MYUBRR);
    joystick_memory_create(joy_x_reg, joy_y_reg, 1023, 1023, (Vec2){0.1, 0.1});
    printf("Startup OK\r\n");
}

void joystick_test_loop(void)
{
    printf("Calibrating...\r\n");
    joystick_memory_calibrate_blocking(&joy, 5000, 1.1);
    printf("Calibration complete.\r\n");
    _delay_ms(1000);

    printf("Starting joystick read loop...\r\n");
    _delay_ms(1000);

    while (1)
    {
        joystick_memory_update(&joy);
        Vec2 pos = joystick_get_position_centered(&joy.joystick);
        printf("X: %.2f, Y: %.2f\r\n", pos.x, pos.y);
        _delay_ms(500);
    }
}
