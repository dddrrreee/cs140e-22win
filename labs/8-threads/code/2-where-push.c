// what address does the first stack operation work on?
#include "rpi.h"
#include "rpi-thread.h"

enum { push_val = 0xdeadbeef };


// implement this assembly routine <0-where-push-asm.S>
// should take a few lines: 
//   - push argument (in r0) onto the stack.
//   - call <after_push> with:
//      - the first argument: sp after you do the push.
//      - second argument: sp before you do the push.
void check_push_asm(uint32_t push_val);

void after_push(uint32_t *sp_after, uint32_t *sp_before) {
    trace("sp_after=%p (val=%x), sp_before=%p (val=%x)\n",
        sp_after, *sp_after, sp_before, *sp_before);

    if(*sp_after == push_val)
        trace("wrote to stack after modifying sp\n");
    else if(*sp_before == push_val)
        trace("wrote to stack before modifying sp\n");
    clean_reboot();
}

void notmain() {
    printk("about to check asm push\n");
    check_push_asm(push_val);
    not_reached();
}
