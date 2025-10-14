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
#include "uart/uart.h"
#include "oled/oled.h"
#include "joystick/joystick.h"
#include "adc/adc.h"
#include "menu/menu.h"

#include "uart/uart.h"
#include <stdio.h>
#include <util/delay.h>

#include <stdio.h>
#include <string.h>
*/

void can_test_setup(void);
void can_test_loop(void);
