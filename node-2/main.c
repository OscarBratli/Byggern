/*
 * main.c - Node 2 Main Program
 * 
 * Ping Pong Game System
 */

#include <stdio.h>
#include "sam.h"
#include "uart.h"
#include "game.h"

int main()
{
    SystemInit();
    WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer

    // Initialize UART for debugging
    uart_init(84000000, 9600);
    
    // Initialize game system
    game_init();
    
    // Main game loop
    while (1) {
        game_loop();
    }
}