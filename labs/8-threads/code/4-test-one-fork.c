// going to run a single thread and exit: makes it easier to test.
#include "rpi.h"
#include "rpi-thread.h"

static void thread_code(void *arg) {
    unsigned *x = arg;

    // check tid
    rpi_thread_t *t = rpi_cur_thread();


	printk("in thread %p, tid=%d with %x\n", t, t->tid, *x);
    assert(t->tid == 1 && *x == 0xdeadbeef);

    printk("success: got to the first thread\n");
    clean_reboot();
}

void notmain() {
    uart_init();
    kmalloc_init_set_start(1024 * 1024, 1024*1024);
    printk("about to fork and run one threa\n");

    unsigned x = 0xdeadbeef;
	rpi_fork(thread_code, &x);
	rpi_thread_start();
    panic("should not get here\n");
}

void print_and_die(void) { panic("should not call\n"); }

