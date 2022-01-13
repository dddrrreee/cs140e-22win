#include "rpi.h"

void notmain(void) {
    printk("hello\n");
    clean_reboot();
}
