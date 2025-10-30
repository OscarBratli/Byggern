/*
 * main.c - Node 2 Main Program
 * 
 * Clean main program for Node 2 (SAM3X8E Arduino Due)
 * Test functions have been moved to test/ for better organization
 */

#include <stdio.h>
#include "sam.h"
#include "uart.h"
#include "test/task7.h"
#include "test/task8.h"



int main()
{
    SystemInit();
    WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer

    // Initialize UART for debugging
    uart_init(84000000, 9600);
   
    printf("\n\n=== Node 2 Starting ===\n");

    // Choose which test to run:
    
    // Option 1: PWM Range Test (for oscilloscope verification)
    // task7_pwm_range_test();
    
    // Option 2: Joystick Servo Control (actual game mode)
    // task7_joystick_servo_control();
    
    // Option 3: IR Sensor Test (check signal before connecting!)
    // task7_ir_sensor_test();
    
    // Option 4: Encoder Test (Task 8 - verify encoder reading)
    // task8_encoder_test();
    
    // Option 5: Joystick Motor Control (Task 8 Step 3 - open-loop motor + servo)
    // task8_joystick_motor_control();
    
    // Option 6: PI Position Control (Task 8 Step 4 - closed-loop with feedback)
    task8_pi_position_control();
    
    // Should never reach here
    while (1) {
        // Main loop - tests run in test functions
    }
}