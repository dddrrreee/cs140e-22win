#include "rpi.h"
void notmain(void) {
    PUT32(0x20200000, 1);
}
