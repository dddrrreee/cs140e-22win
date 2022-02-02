// run N trivial threads that explicitly call exit.
#include "rpi.h"
#include "rpi-thread.h"

void trivial(void* arg) {
    // comment this out to test your implicit exit
    trace("thread %d yielding\n", rpi_cur_thread()->tid);
    rpi_yield();
    trace("thread %d exiting\n", rpi_cur_thread()->tid);
    rpi_exit(0);
}

void notmain(void) {
    uart_init();
    kmalloc_init_set_start(1024 * 1024, 1024 * 1024);

    // make this > 1 to test
    int n = 10;
    for(int i = 0; i < n; i++)
        rpi_fork(trivial, (void*)i);
    rpi_thread_start();
    printk("SUCCESS\n");
    clean_reboot();
}

void print_and_die(void) { panic("should not call\n"); }
