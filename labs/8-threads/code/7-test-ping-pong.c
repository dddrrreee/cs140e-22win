#include "rpi.h"
#include "rpi-thread.h"
#include "libc/circular.h"

struct end {
    cq_t *out, *in;
};

// number of iterations
enum { n = 30 };
static unsigned iter = 0;

void pingpong(void *arg) {
    struct end *e = arg;

    trace("tid=%d, arg=%x\n", rpi_cur_thread()->tid, arg);

    uint8_t v = 0;
    while(v < n) {
        while(!cq_nelem(e->in)) {
            trace("tid=%d yielding\n", rpi_cur_thread()->tid);
            rpi_yield();
        }
        unsigned tid = rpi_cur_thread()->tid;
        v = cq_pop(e->in);
        trace("tid=%d: got=%d, sending=%d\n", tid,v,v+1);
        cq_push(e->out, v+1);
        iter++;
    }
    rpi_exit(0);
}

void notmain(void) {
    uart_init();
    kmalloc_init_set_start(1024 * 1024, 1024*1024);

    cq_t q0,q1;
    cq_init(&q0, 1);
    cq_init(&q1, 1);

    unsigned x = 0;
    cq_push(&q0, 1);
    struct end e0 = {.out = &q0, .in = &q1 },
               e1 = {.out = &q1, .in = &q0 };

    rpi_fork(pingpong, &e0);
    rpi_fork(pingpong, &e1);
    rpi_thread_start();
    printk("iter = %d, n=%d\n", iter,n);
    assert(iter == n+1);
    
    clean_reboot();
}
