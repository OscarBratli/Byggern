#include "cpu_time.h"

volatile long t1_ovf = 0;

ISR(TIMER1_OVF_vect)
{
    t1_ovf++;
}

void cpu_time_init(void)
{
    cli();
    TCCR1A = 0; // normal mode
    TCCR1B = 0;
    TCNT1 = 0;
    TIMSK |= (1 << TOIE1); // enable Timer1 overflow interrupt
    TCCR1B |= (1 << CS11); // prescaler = 8
    sei();
}

long cpu_time_microseconds(void)
{
    long ovf;
    long cnt;
    long s = SREG;
    cli();
    ovf = t1_ovf;
    cnt = TCNT1;
    // handle pending overflow that occurred just after reading TCNT1
    if (TIFR & (1 << TOV1))
    {
        ovf++;
        cnt = TCNT1;
    }
    SREG = s;

    long ticks = (ovf << 16) | cnt; // 16-bit Timer1
    // µs = ticks * (prescaler * 1e6 / F_CPU)
    return (long)((ticks * 8ULL * 1000000ULL) / F_CPU);
}

long cpu_time_milliseconds(void)
{
    return cpu_time_microseconds() / 1000;
}

double cpu_time_seconds(void)
{
    return (double)(cpu_time_microseconds() / 1000000);
}
