// the sw uart should  print hello world 10x if it works.
#include "rpi.h"
#include "sw-uart.h"

void notmain(void) {
    printk("about to use the sw-uart\n");

    // turn off the hw UART so can use the same device.
    uart_disable();

    // use pin 14 for tx, 15 for rx
    sw_uart_t u = sw_uart_init(14,15, 115200);

    for(int i = 0; i < 10; i++)
        sw_uart_putk(&u, "sw_uart: hello world\n");

    // reset to using the hardware uart.
    uart_init();
    printk("done!\n");
}
