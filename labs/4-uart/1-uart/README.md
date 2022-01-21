Implement `uart.c` using the Broadcom document.

  - Before you start: run `make no-uart` --- it should print "hello".

  - `hello.c`: simple test.   It's useful since the bootloader leaves the
     UART on, so you can falsely think your code works.

  - you should be able to run your old tests and see they give identical 
    output if you set the addresses in the same order.

  - you should be able to copy `uart.c` into `libpi/src`, and modify
    `libpi/put-your-src-here.mk` to use that instead of our staff
    `uart.o`.
