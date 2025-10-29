/*
 * main.c - Node 2 Main Program
 * 
 * Clean main program for Node 2 (SAM3X8E Arduino Due)
 * Test functions have been moved to test/task6.c for better organization
 */

#include <stdio.h>
#include "sam.h"
#include "uart.h"
#include "task6.h"



int main()
{
    SystemInit();
    WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer

    // Initialize UART for debugging
    uart_init(84000000, 9600);
   

    // Run the test functions (defined in test/task6.c)
    run_all_tests();
    
    // Should never reach here
    while (1) {
        // Main loop - tests run in run_all_tests()
    }
}