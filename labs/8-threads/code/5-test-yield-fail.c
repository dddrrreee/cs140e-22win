// yield with an empty run queueu.
#include "rpi.h"
#include "rpi-thread.h"

void trivial(void* arg) {
    trace("thread %d yielding\n", rpi_cur_thread()->tid);
    rpi_yield();
    trace("thread %d exiting\n", rpi_cur_thread()->tid);
    rpi_exit(0);
}

void notmain(void) {
    uart_init();
    kmalloc_init_set_start(1024 * 1024, 1024 * 1024);

    rpi_fork(trivial, 0);
    rpi_thread_start();
    printk("SUCCESS\n");
    clean_reboot();
}

void print_and_die(void) { panic("should not call\n"); }
