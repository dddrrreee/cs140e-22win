#include "rpi.h"
#include "trivial-os.h"

#include "pitag.h"

// do not delete these or they go in bss and cstart kills them.
#define PROG_NAME_MAX 128
static char prog_name[PROG_NAME_MAX] = { "do not delete assign\n" };
static void (*code)(void) = (void*)4;

#if 1
#   define lout(args...) do { } while(0)
#else 
#   define lout debug
#endif

// have to do this before you do anything else.
void custom_loader(void) {
    extern char __data_end__;
    extern char __heap_end__;
    uint32_t *bin = (void*)&__data_end__;

    lout("tag = %x\n", bin[0]);
    lout("total bytes=%d\n", bin[1]);
    unsigned b_nbytes = bin[2];
    lout("B bytes=%d\n", b_nbytes);
    lout("LMA computed offset=%d, real offset=%d\n", (uint32_t) bin  - 0x8000, bin[3]);

    unsigned b_addr = bin[4];
    lout("link addr =%x\n", b_addr);

    // asking for trouble.
    assert(b_addr == 0x400000);

    char *p = (char *)&bin[4] + bin[2];
    lout("string = <%s>\n", p);

    lout("copying %d bytes to %x\n", b_nbytes, b_addr);
    memcpy((void*)b_addr, &bin[4], b_nbytes);

    assert(b_addr == 0x400000);

    assert(strlen(p) < PROG_NAME_MAX);
    strcpy(prog_name, p);
    // bad bug: gets killed by cstart
    // prog_name = p;
    lout("prog_name=<%s>\n", p);
    code = (void (*)(void))(b_addr + 4);

    lout("done prog = %x\n", code);
}

struct prog program_get(void) {
    assert(prog_name);
    assert(prog_name[0]);
    assert(code);
    printk("user_code=%x, prog name=<%s>\n", code, prog_name);
    return (struct prog){ .prog_name = prog_name, .user_code = code};

}
