#include "xmem.h"

void xmem_init(void)
{
    MCUCR |= (1 << SRE);  // enable XMEM
    SFIOR |= (1 << XMM2); // mask bits / reduce bus width
}
