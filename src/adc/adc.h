#pragma once

#include "xmem/xmem.h"
#include "utils/utils.h"
#include <util/delay.h>

void adc_init(void);
uint8_t adc_read(uint8_t channel);
