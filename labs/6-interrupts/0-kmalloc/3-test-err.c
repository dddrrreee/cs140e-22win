#include "rpi.h"

void notmain(void) {
    trace("simple check kmalloc(0) fails\n");
    kmalloc_init_set_start(1024 * 1024);

    // should fail
    void *p = kmalloc(0);
    trace_notreached();


    // shouldn't hit.
    trace("kmalloc(0)=%p\n", p);
    assert(!p);
}
