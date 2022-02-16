#include "rpi.h"
#include "trivial-os.h"
#include "breakpoint.h"

static unsigned single_step_faults = 0;

// if get a breakpoint call <brkpt_handler0>
void prefetch_abort_single_step(unsigned pc) {
    output("single_step:prefetch abort at %p\n", pc);
    //debug("setting up a mismatch on %p\n", pc);
    brkpt_mismatch_set(pc);
    //debug("done setting up a mismatch on %p\n", pc);
    single_step_faults++;
}

// maybe should change this so that it doesn't single step
// the switching code?
int single_step_run_fn(int (*user_fn)(void), uint32_t sp) {
    brkpt_mismatch_start();
    int x = user_mode_run_fn(user_fn, sp);
    brkpt_mismatch_stop();
    return x;
}

unsigned single_step_cnt(void) {
    return single_step_faults;
}

