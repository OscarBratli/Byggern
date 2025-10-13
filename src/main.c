#include <util/delay.h>
#include <stdio.h>
#include "spi/spi.h"
#include "ioboard/ioboard.h"
#include "uart/uart.h"

void setup(void)
{
    // Initialize UART for serial communication
    uart_init(MYUBRR);
    
    // Initialize SPI and I/O board
    spi_setup();
    ioboard_init();
    

}

void loop(void)
{
 
}

int main(void)
{
    setup();
    
    while (1) {
        loop();
    }
    return 0;
} 