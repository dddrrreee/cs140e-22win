#include "rpi.h"

void uart_init(void);

void notmain(void) {
    uart_init();
    printk("hello\n");
    clean_reboot();
}
