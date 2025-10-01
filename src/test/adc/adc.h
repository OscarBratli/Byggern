#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "uart/uart.h"
#include <util/delay.h>

int adc_read(int channel);
void adc_test_setup(void);
void adc_test_loop(void);
