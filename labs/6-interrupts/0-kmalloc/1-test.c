#include "rpi.h"

void notmain(void) {
    kmalloc_init_set_start(1024 * 1024);
    trace("checking that kmalloc rounds up to the minumum alignment (4)\n");
    for(int i = 0; i < 10; i++)  {
        void *p = kmalloc_aligned(4,2);
        trace("i=%d: p=%p\n", i,p);
        assert((unsigned)p % 4 == 0);
    }
    trace("success\n");
}
