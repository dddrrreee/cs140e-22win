#include "rpi.h"

void notmain(void) {
    trace("about to check for an error\n");
    kmalloc_init_set_start(1024 * 1024, 1024*1024);
    kmalloc(1024*1024*8+1);
    trace_notreached();
}
