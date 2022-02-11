#include "rpi.h"
#include "atag.h"

void notmain(void) {
    atag_print("pi atags");

    unsigned nbytes = atag_memsize();
    unsigned mb = nbytes / (1024*1024);
    output("mem_nbytes = %d [mb=%d]\n", nbytes, mb);

    if(mb != 128)
        panic("error: default should have 128MB\n", mb);
    output("SUCCESS: done\n");
}
