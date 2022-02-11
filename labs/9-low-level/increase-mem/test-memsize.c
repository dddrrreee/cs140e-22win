// trivial test that we have 498mb of memory
#include "rpi.h"
#include "atag.h"
#include "mbox.h"

enum {  expected_MB = 496 };

void notmain(void) {
    atag_print("pi atags");

    unsigned nbytes = atag_memsize();
    unsigned mb = nbytes / (1024*1024);
    output("mem_nbytes = %d [mb=%d]\n", nbytes, mb);

    // get the memsize using mboxes and corss check
    unimplemented();

    if(mb != expected_MB)
        panic("error: expected %dMB, have %d\n", expected_MB, mb);
    output("SUCCESS: have %dMB of RAM!\n", mb);
}
