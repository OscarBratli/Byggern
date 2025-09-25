#include "joystick.h"

int *joy_x_reg = (int *)0x100; // Example memory-mapped register address for X axis
int *joy_y_reg = (int *)0x101; // Example memory-mapped register address for Y axis

MemoryJoystick joy = joystick_memory_create(joy_x_reg, joy_y_reg, 1023, 1023, (Vec2){0.1, 0.1});

void joystick_test_setup(void)
{
    uart_init(MYUBRR);
    printf("Startup OK\r\n");
}

void joystick_test_loop(void)
{
    while (1)
    {
        joystick_memory_update(&joy);
        Vec2 pos = joystick_get_position_centered(&joy.joystick);
        printf("X: %.2f, Y: %.2f\r\n", pos.x, pos.y);
        _delay_ms(500);
    }
}
