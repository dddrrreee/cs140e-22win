// check that alignment mistakes are detected.
#include "rpi.h"
#include "rpi-interrupts.h"
#include "vector-base.h"

#include "cycle-count.h"

void notmain(void) {

    uint32_t vector[] __attribute__ (( aligned (64) )) = {
        (uint32_t)notmain, 
        (uint32_t)notmain, 
        (uint32_t)notmain, 
        (uint32_t)notmain, 
        (uint32_t)notmain, 
        (uint32_t)notmain, 
        (uint32_t)notmain, 
    };

    printk("setting vector base: should be ok\n");
    vector_base_set(vector);
    printk("setting vector base: should fail\n");
    vector_base_set(&vector[1]);
    
    panic("should not reach here!\n");
}
