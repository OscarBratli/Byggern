#include "test/can/can.h"
#include "test/adc/adc.h"
#include "test/echo/echo.h"
#include "test/sram/sram.h"
#include "test/menu/menu.h"
#include "test/game_menu/game_menu.h"

void setup(void) 
{
    can_test_setup();    // Enable CAN setup (includes joystick init)
    game_menu_init();    // Initialize game menu with OLED display
}

void loop(void)
{
    game_menu_loop();    // Handle menu display and joystick CAN communication
}
 
int main(void)
{
    setup();  
    
    while (1) {
        loop(); 
    }
    return 0; 
}
