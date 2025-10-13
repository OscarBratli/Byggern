#pragma once

#include "driver/driver.h"
#include "uart/uart.h"
#include "utils/utils.h"
#include <util/delay.h>
#include <stdio.h>

/**
 * Initialize the driver test.
 * This sets up UART for printf output and initializes the joystick/slider driver.
 */
void driver_test_setup(void);

/**
 * Main test loop that continuously reads and displays joystick and slider positions.
 * This function demonstrates reading both raw ADC values and processed positions.
 */
void driver_test_loop(void);
