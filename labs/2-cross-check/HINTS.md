We will fill these in as they come up.

To make it easy to compare, just have:

    if(pin >= 32)
        return;

At the start of `gpio_set_on`, `gpio_set_off`, `gpio_set_input`,
`gpio_set_output`.


For `gpio_read` it arguably makes sense to return something obviously
wrong, so:

    if(pin >= 32)
        return -1;


### Hints for 1-fake-pi

You can run things by hand to see better what is going on.

        % cd 1-fake-pi/tests
        % make

        # run one of the 0 tests by hand
        % ./0-put
        calling pi code
        TRACE:PUT32:20200000:1
        pi exited cleanly
        TRACE:1 calls to random

        # what is it doing?
        % cat 0-put.c
        #include "rpi.h"
        void notmain(void) {
            PUT32(0x20200000, 1);
        }
 

An easy way to get a difference is to call `fake_random` a different
number of times.  The `fake-pi` code will print out the number of calls
so check that first.

There are a couple reference outputs:

        % cat 1-fake-pi/tests/0-get.*ref
        TRACE:GET32:20200000:327b23c6
        TRACE:2 calls to random


If `ls` lists the output in the same order, you can run `cksum` on the
`cksum` of everything for a single way to compare everything:

        % make emitall
        % ls *.out
        0-get.out      1-blink.out	     1-gpio-set-on.out	    3-unix-gpio-test.out
        0-put-get.out  1-gpio-set-input.out  1-gpio-set-output.out  3-unix-test-put-get.out
        0-put.out      1-gpio-set-off.out    2-blink.out	    4-unix-test-gpio.out
        cksum *.out | cksum
        439198485 380


As a sanity on the file size:

        % wc *.out 
        2     5    54 0-get.out
        7    10   162 0-put-get.out
        2     5    47 0-put.out
        23    26   644 1-blink.out
        3     6    84 1-gpio-set-input.out
        2     5    52 1-gpio-set-off.out
        2     5    52 1-gpio-set-on.out
        3     6    84 1-gpio-set-output.out
        45    48  1264 2-blink.out
        2     6    62 3-unix-gpio-test.out
        256   256  7638 3-unix-test-put-get.out
        704   704 20654 4-unix-test-gpio.out
        1051  1082 30797 total


### For 2-trace

These are what I got (you can get different b/c of hardware issues):

    % cksum *.out
    2962364034 35 0-gpio-read.bin.out
    3159539606 56 0-gpio-set-input.bin.out
    3179942673 56 0-gpio-set-output.bin.out
    610433833 33 0-gpio-write.bin.out
    4294967295 0 1-blink.bin.out
    3754849163 392 1-set-n-input.bin.out
    2100720877 414 1-set-n-output.bin.out
    4164008761 928 2-set-stuff.bin.out
    3731391281 716 prog-1-blink.bin.out
    486978757 1432 prog-2-blink.bin.out

    % cksum *.out|cksum
    1435331407 358

