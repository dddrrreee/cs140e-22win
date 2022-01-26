/* trivial, sleazy test of kmalloc: don't modify!  */
#include "rpi.h"

void notmain() {
    uart_init();

    trace("TRACE: setting malloc start to 1mb\n");
    kmalloc_init_set_start(1024 * 1024, 1024*1024*8);
    trace("heap starts at %x\n", kmalloc_heap_ptr());

    void *heap0 = kmalloc(1);
    assert(heap0 == (void*)(1024*1024));

    // sleazy check that assumes how kmalloc works.
    unsigned *start = heap0, *last = start;
    trace("start=%x\n", start);
    for(unsigned i = 0; i < 20; i++) {
        unsigned *u = kmalloc(1);

        trace("u=%p\n", u);

        unsigned v = (i << 24 | i << 16 | i << 8 | i);
        *u = v;

    }

    unsigned *u = start+1;
    for(unsigned i = 0; i < 20; i++) {
        unsigned v = (i << 24 | i << 16 | i << 8 | i);
        
        demand(*u == v, "*u =%x, should be %x\n", *u,v);
        u += 1;
    }

    void *p = kmalloc_aligned(1,64);
    demand((unsigned)p % 64 == 0, bug in kmalloc_aligned);

    kfree_all();

    void *heap1 = kmalloc(1);
    trace("heap after free_all=%x\n", heap1);
    assert(heap1 == (void*)(1024*1024));

    demand(heap0 == heap1, did not reset the heap correctly!);
    demand(start+1 == kmalloc(1), did not reset the heap correctly!);

    trace("successful test\n");
	clean_reboot();
}
