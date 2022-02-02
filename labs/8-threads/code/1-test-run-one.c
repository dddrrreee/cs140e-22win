/* simplest test: run a single thread. */
#include "rpi.h"
#include "rpi-thread.h"

// trivial first thread: does not block, returns.
// makes sure you pass in the right argument.
static void thread_code(void *arg) {
    unsigned *x = arg;

    // check tid
    rpi_thread_t *t = rpi_cur_thread();

	printk("in thread %p, tid=%d with %x\n", t, t->tid, *x);
    demand(rpi_cur_thread()->tid == *x, 
                "expected %d, have %d\n", t->tid,*x+1);

    *x += 1;
}

void notmain() {
    kmalloc_init_set_start(1024 * 1024, 1024*1024);
    printk("about to run 1 thread\n");

    int x = 1;
	rpi_fork(thread_code, &x);
	rpi_thread_start();
    assert(x == 2);

    printk("SUCCESS!\n");
	clean_reboot();
}

void print_and_die(void) { panic("should not call\n"); }
