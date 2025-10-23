#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "can/can.h"
#include <avr/pgmspace.h>
#include "mcp2515/mcp2515.h"

/*
#include <stdio.h>
#include <stdlib.h>
#include "uart/uart.h"
#include "xmem/xmem.h"

#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include "spi/spi.h"
#include "ioboard/ioboard.h"
*/
void can_test_setup(void);
void can_test_loop(void);
void can_test_loop_continuous(void);

