#include "rpi.h"

void delay(unsigned ticks) {
    while(ticks-- > 0)
        asm("add r1, r1, #0");
}

unsigned timer_get_usec(void) {
	return GET32(0x20003004);
}

void delay_us(unsigned us) {
    unsigned rb = timer_get_usec();
    while (1) {
        unsigned ra = timer_get_usec();
        if ((ra - rb) >= us) {
            break;
        }
    }
}

void delay_ms(unsigned ms) {
	delay_us(ms*1000);
}
