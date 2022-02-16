#ifndef __TRIVIAL_OS_H__
#define __TRIVIAL_OS_H__

#include "syscalls.h"

// from loader.c
struct prog {
    const char *prog_name;
    void (*user_code)(void);
};
struct prog program_get(void);


// assembly routine: run <fn> at user level with stack <sp>
//    XXX: visible here just so we can use it for testing.
int user_mode_run_fn(int (*fn)(void), uint32_t sp);

extern unsigned sys_verbose_p;
#define sys_debug(msg...) do { if(sys_verbose_p) output(msg); } while(0)


/*******************************************************
 * part 2: single step
 */

// run a single routine <user_fn> in single step mode using stack <sp>
int single_step_run_fn(int (*user_fn)(void), uint32_t sp);

// return the number of instructions we single stepped.
//  (just count the faults)
unsigned single_step_cnt(void);

// assembly routine: run <fn> at user level with stack <sp>
//    XXX: visible here just so we can use it for testing.
int user_mode_run_fn(int (*fn)(void), uint32_t sp);

/*******************************************************
 * part 3/4: pc equiv
 */

// will only print if running in equiv mode.
void equiv_print(const char *msg);

// run user_fn in single stepping mode at user level using stack <sp>
// n_print = the first n instructions (for debugging)
int equiv_run_fn(int (*user_fn)(void), uint32_t sp, unsigned n_print);

#endif
