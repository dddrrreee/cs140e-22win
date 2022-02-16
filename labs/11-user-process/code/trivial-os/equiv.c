#include "rpi.h"
#include "trivial-os.h"
#include "libc/fast-hash32.h"
#include "cpsr-util.h"

#include "breakpoint.h"

// context of an equivalance job.
typedef struct {
    uint32_t n_inst,    // total instructions run equiv checking
             pc_hash,   // hash of pc values
             reg_hash,  // hash of register values.
             print_first_n;    // print out the first n pc/reg values for debugging
} equiv_ctx_t;

static equiv_ctx_t cur_ctx;

// should we start tracing yet?
static int in_user = 0;

// figure out when we actually jumped to user code.
static int user_pc(uint32_t pc) {
    extern char __heap_start__;
    return (pc >= (uint32_t)&__heap_start__);
}

// if get a breakpoint call <brkpt_handler0>
void prefetch_abort_equiv_pc(unsigned pc) {
    unimplemented();

    // if you are in user code, hash the pc.
    // 
    // if(in_user) {
    //    c->pc_hash = fast_hash_inc(&pc, sizeof pc, hash);
}

// if get a breakpoint call <brkpt_handler0>
void prefetch_abort_equiv(uint32_t *regs, uint32_t spsr, uint32_t pc) {
    unimplemented();
    // if in user mode, hash all the registers and spsr.
}


int equiv_run_fn(int (*user_fn)(void), uint32_t sp, unsigned n_print) {
    in_user = 0;
    memset(&cur_ctx, 0, sizeof cur_ctx);
    cur_ctx.print_first_n = n_print;
    assert(mode_is_super());


    brkpt_mismatch_start();
    int x = user_mode_run_fn(user_fn, sp);
    not_reached();
}

// user can do this too.
void equiv_print(const char *msg) {
    equiv_ctx_t c = cur_ctx;

    if(!c.n_inst)
        return;
    printk("%s\n", msg);
    if(c.n_inst)
        trace("EQUIV:\tnumber instructions = %d\n", c.n_inst);
    if(c.reg_hash)
        trace("EQUIV:\treg hash = %x\n", c.reg_hash);
    // don't print pc hash if we print reg hash.
    else if(c.pc_hash)
        trace("EQUIV:\tpc hash = %x\n", c.pc_hash);
}
