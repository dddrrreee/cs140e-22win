// support.
#include <stdint.h>

#define die(x) do { user_putk(x); sys_exit(1); } while(0)

void user_putk(const char *msg);
void user_printk(const char *fmt, ...);

#include "syscalls.h"

// fake-user-level-asm.S
int syscall_invoke_asm(uint32_t sysno, ...);

#define sys_putc(x)     syscall_invoke_asm(SYS_PUTC, x)
#define sys_put_hex(x)  syscall_invoke_asm(SYS_PUT_HEX, x)




void sys_exit(int code);

