/* tests that implicit exit works */

#include "rpi.h"
#include "rpi-thread.h"

static unsigned thread_count, thread_sum;

// trivial first thread: does not block, explicitly calls exit.
static void thread_code(void *arg) {
    unsigned *x = arg;

    // check tid
    rpi_thread_t *t = rpi_cur_thread();

	trace("in thread %p, tid=%d with %x\n", t, t->tid, *x);
    demand(rpi_cur_thread()->tid == *x+1, 
                "expected %d, have %d\n", t->tid,*x+1);

	thread_count ++;
	thread_sum += *x;

    // no exit.
}

void notmain() {
    uart_init();
    kmalloc_init_set_start(1024 * 1024, 1024*1024);

    printk("about to test summing of 30 threads\n");

    // change this to increase the number of threads.
	int n = 30;
	thread_sum = thread_count = 0;

    unsigned sum = 0;
	for(int i = 0; i < n; i++)  {
        int *x = kmalloc(sizeof *x);
        sum += *x = i;
		rpi_fork(thread_code, x);
    }
	rpi_thread_start();

	// no more threads: check.
	printk("count = %d, sum=%d\n", thread_count, thread_sum);
	assert(thread_count == n);
	assert(thread_sum == sum);
    printk("SUCCESS!\n");
	clean_reboot();
}
void print_and_die(void) { panic("should not call\n"); }

