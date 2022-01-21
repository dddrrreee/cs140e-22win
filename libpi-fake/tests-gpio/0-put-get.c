#include "rpi.h"
void notmain(void) {
    unsigned n = 2;
    for(int i = 0 ; i < n; i++) {
        PUT32(0x20200000, i);
        unsigned x = GET32(0x20200000);
        PUT32(0x20200000 + i*4, x);
    }
}
