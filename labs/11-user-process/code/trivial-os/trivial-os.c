// engler: cs140e, 2020.
//
// tiny example of both the user-level side and kernel level side of how
// we'll do system calls:
//  1. pass the number in r0.   loading from the system call instruction makes
//     some things more awkward.
//  2. the other arguments in r1,r2,r3 --- for the moment we don't need more.
//  3. use the vector base register to relocate the table.
//  4. the exception table uses private names better to minimize conflicts.
//
// it's not really much different than the interrupt lab: just review and make sure
// you see how we are passing things.
#include "rpi.h"
#include "rpi-interrupts.h"
#include "vector-base.h"
#include "reg-util.h"
#include "trivial-os.h"

// change this to run the different parts.
// you should not have to modify anything else.
enum {
    PART0_RUN_RAW = 0,
    PART1_RUN_USER,
    PART2_RUN_SINGLE,
    PART3_RUN_PC_EQUIV,
    PART4_RUN_REG_EQUIV
};
static int part = 5;
//static int part = PART2_RUN_SINGLE; // PART4_RUN_REG_EQUIV;
// static int part = PART4_RUN_REG_EQUIV;
//static int part = PART3_RUN_PC_EQUIV;

// set to 1 to print stuff.
unsigned sys_verbose_p = 0;

static const char *prog_name = "hello";

/*************************************************************************
 * code for our simplistic kernel level side
 * we start with two system calls.
 */

// kernel level side
uint32_t do_syscall(uint32_t sysnum, uint32_t a0, uint32_t a1, uint32_t a2) {
    sys_debug("syscall=%d, a0=%x,a1=%x,a2=%x\n", sysnum, a0,a1,a2);
    switch(sysnum) {
    case SYS_PUT_HEX: 
        if(sys_verbose_p) printk("USER: %x\n", a0);  
        else printk("%x", a0);
        return 0;
    case SYS_PUTC: 
        if(sys_verbose_p) printk("USER: %c\n", a0);  
        else printk("%c", a0);
        return 0;
    case SYS_EXIT: 
        printk("%s: sys_exit(%d): going to reboot\n", prog_name, a0);
        printk("part=%d\n", part);
        if(part > 2)
            equiv_print("equiv values");
        clean_reboot();
    default: panic("undefined system call: %d\n", sysnum);
    }
}

void notmain(void) {
    extern uint8_t part1_swi_vec, part2_single_step_vec, part3_equiv_pc_vec,
        part4_equiv_vec;

    printk("kernel: stack is roughly at: %x\n", reg_get_sp());

    // extern void notmain_test(void);
    // int (*code)(void) = (void*)notmain_test;

    // will implement this part when you do equiv checking.
    struct prog p = program_get();
    prog_name = p.prog_name;
    int (*code)(void) = (void*)p.user_code;


    // initially just run code from our own binary.
    switch(part) {
    case PART0_RUN_RAW: 
        // this should just work.
        // base case: just call the user code directly.
        vector_base_set(&part1_swi_vec);
        code();
        not_reached();
    case PART1_RUN_USER: 
        // implement switch user.
        // now do the same but run at user level
        vector_base_set(&part1_swi_vec);
        user_mode_run_fn(code, STACK_ADDR2);
        not_reached();
    case PART2_RUN_SINGLE: 
        // this should just work.
        // ... + run in single step mode
        vector_base_set(&part2_single_step_vec);
        single_step_run_fn(code, STACK_ADDR2);
        not_reached();
    case PART3_RUN_PC_EQUIV: 
        // ... + compute hashes for pc
        vector_base_set(&part3_equiv_pc_vec);
        equiv_run_fn(code, STACK_ADDR2, 10);
        not_reached();
    case PART4_RUN_REG_EQUIV: 
        // ... + compute hashes for all regs.
        vector_base_set(&part4_equiv_vec);
        equiv_run_fn(code, STACK_ADDR2, 10);
        not_reached();
    default: panic("bad part=%d\n", part);
    }
    not_reached();
}
