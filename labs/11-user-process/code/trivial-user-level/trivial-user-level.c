/*************************************************************************
 * engler: cs140e: code for our simplistic user-level side we start 
 * with three system calls.
 */
#include "rpi.h"
#include <stdarg.h>

#include "syscalls.h"
#include "reg-util.h"
#include "libos.h"

#if 0

// simple wrappers that invoke the assembly side with the 
// right system call number.
int syscall_invoke_asm(uint32_t sysno, ...);
#define sys_putc(x)     syscall_invoke_asm(SYS_PUTC, x)
#define sys_put_hex(x)  syscall_invoke_asm(SYS_PUT_HEX, x)
#define sys_exit(x)     syscall_invoke_asm(SYS_EXIT, x)
#endif

void sys_exit(int code) { 
    syscall_invoke_asm(SYS_EXIT, code);
}

// #define die(x) do { user_putk(x); sys_exit(1); } while(0)

void user_putk(const char *msg) {
    while(*msg)
        sys_putc(*msg++);
}

// simple printk that only takes %x as a format 
// we need a seperate routine since when actually at user level
// can't call kernel stuff.
void user_printk(const char *fmt, ...) {
    unsigned chr;
    va_list ap;

    va_start(ap, fmt);
    while((chr = *fmt++)) {
        switch(chr) {
        case '%': 
            if((chr = *fmt++) != 'x') 
                die("bad character\n");
            sys_put_hex(va_arg(ap, unsigned));
            break;
        default: sys_putc(chr);
        }
    }
    va_end(ap);
}

#if 0
// fake "user program" that we use as a first step --- it's linked
// in the "kernel" address space = it's easier to interpret 
// addresses.
void notmain_test(void) {
    sys_exit(0);

    user_putk("hello world\n");
    user_printk("user: stack is roughly at %x\n", reg_get_sp());
    user_printk("user: cpsr=%x\n", reg_get_cpsr());
    switch(reg_get_mode()) {
    case USER_MODE: user_putk("USER MODE!\n"); break;
    case SUPER_MODE: user_putk("SUPER MODE!\n"); break;
    default: die("illegal mode\n");
    }
    sys_exit(0);
}
#endif
