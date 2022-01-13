#ifndef __CYCLE_COUNT_H__
#define __CYCLE_COUNT_H__
// use r/pi cycle counters so that we can do tighter timings.

#ifndef RPI_UNIX
// must do init first.
static inline void cycle_cnt_init(void)  {
    unsigned in = 1;
    asm volatile("MCR p15, 0, %0, c15, c12, 0" :: "r"(in));
}
// read.  should add a write().
static inline unsigned cycle_cnt_read(void) {
	unsigned out;
  	asm volatile ("MRC p15, 0, %0, c15, c12, 1" : "=r"(out));
    return out;
}
#else

// if we are doing fake testing, have to provide fake versions of these.
void cycle_cnt_init(void);
unsigned cycle_cnt_read(void);

#endif

//
// XXX need to investigate more --- without the barrier it appears
// gcc moves stuff around.  for timing code: do we have to be 
// smarter about using barriers?
#define TIME_CYC(_fn) ({                \
    unsigned _s = cycle_cnt_read();     \
    gcc_mb();\
    _fn;                                \
    gcc_mb();\
    unsigned _e = cycle_cnt_read();     \
    ((_e - _s) - 0);                    \
})

#define TIME_CYC_10(_fn) ({                     \
    unsigned _s = cycle_cnt_read();             \
    _fn; _fn; _fn; _fn; _fn;                    \
    _fn; _fn; _fn; _fn; _fn;                    \
    unsigned _e = cycle_cnt_read();             \
    ((_e - _s)-0);                              \
})

#define TIME_CYC_PRINT(_msg, _fn) ({                          \
    unsigned _t = TIME_CYC(_fn);                                \
    printk("%s: %d cycles <%s>\n", _msg, _t, _XSTRING(_fn));    \
    _t;                                                         \
})

#define TIME_CYC_PRINT10(_msg, _fn) do {                            \
    unsigned _t = TIME_CYC_10(_fn);                                 \
    printk("%s: %d cycles (x10) <%s>\n", _msg, _t, _XSTRING(_fn));  \
} while(0)

#include "libc/math-helpers.h"

static inline void cyc_indent(const char *msg) {
    for(; *msg; msg++) {
        if(*msg == ' ' || *msg == '\t')
            printk("%c", *msg);
        else
            return;
    }
}

#define TIME_CYC_DEADLINE(_msg, _fn, _target) ({            \
    unsigned _t = TIME_CYC(_fn);                            \
    uint32_t diff = abs_diff_u(_t, _target);                \
    printk("%s: <%s>\n", _msg, _XSTRING(_fn));                                  \
    cyc_indent(_msg); printk("    %u cycles measured\n", _t);      \
    cyc_indent(_msg); printk("    %u cycle target\n", _target);                     \
    cyc_indent(_msg); printk("    %u cycle error (%u nanosec)\n",             \
             diff, cycles_to_nanosec(diff));                \
    _t;                                                     \
})

#endif
