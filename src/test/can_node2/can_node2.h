#ifndef CAN_NODE2_TEST_H
#define CAN_NODE2_TEST_H

#include <stdint.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "uart/uart.h"
#include "can/can.h"
#include "mcp2515/mcp2515.h"

// Setup function for Node 1 to Node 2 CAN communication test
void can_node2_test_setup(void);

// Loop function that sends messages to Node 2
void can_node2_test_loop(void);

#endif // CAN_NODE2_TEST_H
