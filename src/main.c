#include "test/can/can.h"

void setup(void) 
{
    can_test_setup();
}

void loop(void)
{
    can_test_loop();
}

int main(void)
{
    setup();
    
    while (1) {
        loop();
    }
    return 0; 
}
