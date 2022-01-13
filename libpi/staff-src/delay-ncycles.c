#include "rpi.h"

// roughly 2-3x the number of cyles.  dunno if we care.  can read cycle count from
// cp15 iirc.
void delay_cycles(unsigned ticks) {
#ifndef RPI_UNIX
    while(ticks-- > 0)
        asm("add r1, r1, #0");
#else
    void dummy_fn(void);
    while(ticks-- > 0)
        dummy_fn();
#endif
}


