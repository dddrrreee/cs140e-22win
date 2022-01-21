// trivial little fake r/pi system that will allow you to debug your
// gpio.c code. 
#include "fake-pi.h"

static mem_t fake_mem = {};

#if 0
#define random fake_random

// there is no assembly or other weird things in rpi.h so we can
// include and use it on your laptop even tho it is intended for
// the pi.
#include "rpi.h"

// macro tricks to make error reporting easier.
#define output(msg, args...) printf(msg, ##args )
#define trace(msg, args...) fprintf(stderr, "TRACE:" msg, ##args )

#define panic(msg, args...) do {                                         \
    printf("PANIC:%s:%s:%d:", __FILE__, __FUNCTION__, __LINE__);   \
    printf(msg, ##args);                                                \
    exit(1);                                                        \
} while(0)
#endif

// main pi-specific thing is a tiny model of device
// memory: for each device address, what happens when you
// read or write it?   in real life you would build this
// model more succinctly with a map, but we write everything 
// out here for maximum obviousness.

#define GPIO_BASE 0x20200000
enum {
    gpio_fsel0 = (GPIO_BASE + 0x00),
    gpio_fsel1 = (GPIO_BASE + 0x04),
    gpio_fsel2 = (GPIO_BASE + 0x08),
    gpio_set0  = (GPIO_BASE + 0x1C),
    gpio_clr0  = (GPIO_BASE + 0x28),
    gpio_lev0  = (GPIO_BASE + 0x34),
    // p18
    uart_mu_stat_reg = 0x20215064,
    // p11
    uart_mu_io_reg = 0x20215040
};

#if 0
static unsigned 
        gpio_fsel0_v,
        gpio_fsel1_v,
        gpio_fsel2_v,
        gpio_set0_v,
        gpio_clr0_v;
#endif

void put32(volatile void *addr, uint32_t v) {
    PUT32((uint32_t)(uint64_t)addr, v);
}

// same, but takes <addr> as a uint32_t
void PUT32(uint32_t addr, uint32_t v) {
    mem_ent_t *m;
    switch(addr) {
#if 0
    case gpio_fsel0: gpio_fsel0_v = v;  break;
    case gpio_fsel1: gpio_fsel1_v = v;  break;
    case gpio_fsel2: gpio_fsel2_v = v;  break;
    case gpio_set0:  gpio_set0_v  = v;  break;
    case gpio_clr0:  gpio_clr0_v  = v;  break;
#endif
    case gpio_lev0:  panic("illegal write to gpio_lev0!\n");
    //case uart_mu_io_reg:
        // // fallthrough
        // putchar(v & 0xff);
    default: 
        // change to detect if they go out of gpio?
        if((m = mem_lookup(&fake_mem,addr)))
            m->val = v;
        else
            mem_insert(&fake_mem,addr,v);
        break;
    }
    trace("PUT32:%x:%x\n", addr,v);
}

uint32_t get32(const volatile void *addr) {
    return GET32((uint32_t)(uint64_t)addr);
}

/*
 * most <addr> we treat as normal memory: return the 
 * value of the last write.
 *
 * otherwise we do different things: time, status regs,
 * input pins.
 */
uint32_t GET32(uint32_t addr) {
    unsigned v;
    mem_ent_t *m;

    switch(addr) {
#if 0
    case gpio_fsel0: v = gpio_fsel0_v; break;
    case gpio_fsel1: v = gpio_fsel1_v; break;
    case gpio_fsel2: v = gpio_fsel2_v; break;
    case gpio_set0:  v = gpio_set0_v;  break;
    case gpio_clr0:  v = gpio_clr0_v;  break;
#endif
    // to fake a changing environment, we want gpio_lev0 to 
    // change --- we just use a uniform random coin toss, 
    // but you would bias these as well or make them more 
    // realistic by reading from a trace from a run on 
    // the raw hardware, correlating with other pins or 
    // time or ...
    case uart_mu_stat_reg: 
    case gpio_lev0:  v = fake_random();  break;
    default: 
        // we could warn if not written before read and its not
        // one of the special addresses.
        //
        // XXX: perhaps warn if out of the GPIO range.
        if((m = mem_lookup(&fake_mem, addr)))
            v = m->val;
        else
            mem_insert(&fake_mem, addr, v = fake_random());
        break;
    }
    trace("GET32:%x:%x\n", addr,v);
    return v;
}

// don't need to do anything.  would be better to not have
// delay() in the header so we could do our own version and
// (for example) print out the number of ticks used.
void (nop)(void) {
}

// don't even trACK
uint32_t DEV_VAL32(uint32_t x) { return x; }

#if 0
void fake_init(void) {
    fake_random_init();
}

// initialize "device memory" and then call the pi program
//
// initialize and say what is the max and min allowed.
int main(int argc, char *argv[]) {
    fake_init();

    void notmain(void);
#if 0
    // initialize "device memory" to random values.
    // extension: do multiple iterations, initializing to
    // different values each time.
    gpio_fsel0_v = random();
    gpio_fsel1_v = random();
    gpio_fsel2_v = random();
    gpio_set0_v  = random();
    gpio_clr0_v  = random();
#endif

    // extension: run in a subprocess to isolate
    // errors.
    output("calling pi code\n");
    notmain();
    output("pi exited cleanly\n");
    trace("%d calls to random\n", fake_random_calls());
    return 0;
}
#endif
