#pragma once

#include <avr/pgmspace.h>
#include "uart/uart.h"
#include <stdlib.h>
#include "adc/adc.h"
#include "uart/uart.h"
#include <util/delay.h>

void adc_test_setup(void);
void adc_test_loop(void);
