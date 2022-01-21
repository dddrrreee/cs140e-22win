// low level print that makes a bit easier to debug.
#include "rpi.h"

static void my_putk(char *s) {
    for(; *s; s++)
        uart_putc(*s);
}

void notmain(void) {
    // hack to make sure aux is off.
    dev_barrier();
    PUT32(0x20215004, 0);
    dev_barrier();

    for(unsigned i = 0; i < 10; i++) {
        uart_init();
        my_putk("hello world\n");
        uart_disable();
    }
    uart_init();
}
