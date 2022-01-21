Test your bootloader with the new uart.

Check your code:

  1. make sure the tests in `3-bootloader/tests` work.
  2. copy `gpio.c` and `uart.c`: to `libpi/src` and change
     `libpi/put-your-src-here.mk` if you haven't already.
  3. copy your `3-bootloader/pi-side/get-code.h` here
  4. remake and install on your SD card.
  5. the tests in `3-bootloader` should still pass.


