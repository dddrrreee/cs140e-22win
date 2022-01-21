// simple test for your put/get implementation.
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


#include "rpi.h"
#include "fake-pi.h"

#define stringify(x) #x
#define test(fn) do { \
    printf("testing <%s>\n", stringify(fn)); \
    fn; \
} while(0)

int main(int argc, char *argv[]) {
    fake_random_init();

    // uncomment these out one at a time and check with your 
    // partner.  easiest to debug a single call rather than the 
    // more involved sequences of later tests.
    test(gpio_set_off(20));

#if 0
    test(gpio_set_on(20));
    test(gpio_read(20));
    test(gpio_set_input(20));
    test(gpio_set_output(20));
#endif
    trace("done: called random %d times\n", fake_random_calls());
    return 0;
}
