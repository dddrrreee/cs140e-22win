#include "rpi.h"

void notmain(void) {
    trace("simple check for kmalloc\n");
    kmalloc_init_set_start(1024 * 1024);
    trace("kmalloc(1)=%p\n", kmalloc(1));
    trace("kmalloc(1)=%p\n", kmalloc(1));
    trace("kmalloc(1)=%p\n", kmalloc(1));

    trace("done\n");
}
