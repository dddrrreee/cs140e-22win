#include "rpi.h"
#include "mbox.h"

void notmain(void) { 
    kmalloc_init();

    enum { OneMB = 1024 * 1024 };

    printk("mailbox serial number = %llx\n", rpi_get_serialnum());
    uint32_t size = rpi_get_memsize();
    output("mailbox physical mem: size=%d (%dMB)\n", size, size/OneMB);

    // if you want to try to set the CPU frequency.  i couldn't get the
    // measured cpu speed to go up, however.

#if 0
    uint32_t curhz = rpi_clock_curhz_get(0x3);   
    uint32_t maxhz = rpi_clock_maxhz_get(0x3);   

    output("cur hz=%d, maxhz = %d\n", curhz, maxhz);
    rpi_clock_hz_set(0x3, curhz+1); 
    output("set!\n");

    output("new hz=%d\n", rpi_clock_curhz_get(3)); 

    output("setting to max hz\n");
    uint32_t newhz = rpi_clock_hz_set(0x3, curhz+ 512 * 1024); 
    output("new hz=%d [mbox=%d]\n", newhz, rpi_clock_curhz_get(3)); 
#endif
    // some other useful things.
 //   output("board model = 0x%x\n", rpi_get_model());
//    output("board revision= 0x%x\n", rpi_get_revision());
}
