#include "rpi.h"

void my_putk(const char *msg) {
    while(*msg)
        uart_putc(*msg++);
}

void notmain(void) {
    uart_init();
    my_putk("hello\n");
}
