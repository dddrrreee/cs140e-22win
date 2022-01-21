#include "fake-pi.h"

// should print something.
void delay_us(unsigned usec) { 
    trace("delay usec: %d\n", usec);
}
