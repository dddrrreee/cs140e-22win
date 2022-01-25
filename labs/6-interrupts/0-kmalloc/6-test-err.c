#include "rpi.h"

void notmain(void) {
    trace("about to check for an error\n");
    kmalloc_init();
    // allocate 8mb+1: should fail.
    kmalloc(1024*1024*8+1);
    trace_notreached();
}
