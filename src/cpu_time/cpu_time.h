#include <time.h>
#include <avr/io.h>
#include "utils/utils.h"
#include <avr/interrupt.h>

/**
 * Initializes the cpu time module on Timer1.
 */
void cpu_time_init(void);

/**
 * Returns the number of seconds since the cpu started.
 */
double cpu_time_seconds(void);

/**
 * Returns the number of milliseconds since the cpu started.
 */
long cpu_time_milliseconds(void);

/**
 * Returns the number of microseconds since the cpu started.
 */
long cpu_time_microseconds(void);
