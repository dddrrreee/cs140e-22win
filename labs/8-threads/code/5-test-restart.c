// run the threads package N times.
#include "rpi.h"
#include "rpi-thread.h"

static void thread_code(void *arg) {
    unsigned *x = arg;

    // not sure if we should reset the tid across runs?
    rpi_thread_t *t = rpi_cur_thread();

	trace("in thread %p, tid=%d with %x\n", t, t->tid, *x);
    assert(*x == 0xdeadbeef);
//    assert(t->tid == 1 && *x == 0xdeadbeef);

    printk("success: got to the first thread\n");
    rpi_exit(0);
}

void notmain() {
    uart_init();
    kmalloc_init_set_start(1024 * 1024, 1024*1024);

    // make sure starting threads with no threads works.
    for(int i = 0; i < 10; i++) {
        trace("%d: about to start threads package wth nothing\n", i);
	    rpi_thread_start();
    }

    // run the threads package N times.
    for(int i = 0; i < 10; i++) {
        trace("%d: about to fork and run one thread\n", i);
        
        unsigned x = 0xdeadbeef;
	    rpi_fork(thread_code, &x);
	    rpi_thread_start();
    }
}

void print_and_die(void) { panic("should not call\n"); }
