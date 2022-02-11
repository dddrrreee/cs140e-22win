#include "rpi.h"
#include "cycle-count.h"

unsigned cyc_per_sec(void) {
    unsigned cyc_start = cycle_cnt_read();

    unsigned s = timer_get_usec();
    while((timer_get_usec() - s) < 1000*1000)
        ;

    unsigned cyc_end = cycle_cnt_read();
    return cyc_end - cyc_start;
}
