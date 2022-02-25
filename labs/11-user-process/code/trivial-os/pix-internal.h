#ifndef __PIX_ENV_H__
#define __PIX_ENV_H__
#include "rpi.h"

// ***** DO NOT MODIFY THIS FILE!  We're going to update it. *****
// ***** DO NOT MODIFY THIS FILE!  We're going to update it. *****
// ***** DO NOT MODIFY THIS FILE!  We're going to update it. *****
// ***** DO NOT MODIFY THIS FILE!  We're going to update it. *****
// ***** DO NOT MODIFY THIS FILE!  We're going to update it. *****
// ***** DO NOT MODIFY THIS FILE!  We're going to update it. *****
// ***** DO NOT MODIFY THIS FILE!  We're going to update it. *****
// ***** DO NOT MODIFY THIS FILE!  We're going to update it. *****
// ***** DO NOT MODIFY THIS FILE!  We're going to update it. *****
// ***** DO NOT MODIFY THIS FILE!  We're going to update it. *****


/*********************************************************************
 * simple process structure
 */

// context of an equivalance job.
// duplicates the struct in equiv.c : cleanup.
typedef struct {
    uint32_t n_inst,    // total instructions run equiv checking
             pc_hash,   // hash of pc values
             reg_hash,  // hash of register values.
             print_first_n;    // print out the first n pc/reg values for debugging
} pix_equiv_t;

// reserve asid=1 for kernel, asid=2 and above for user.
// perhaps should decouple these, since there are tricks
// that can be used.
enum {
    kernel_asid = 1,
    first_user_asid = 2
};

// reg_save offsets for the different registers.
enum {
    SP_OFF = 13,
    PC_OFF = 15,
    SPSR_OFF = 16,
};

typedef struct first_level_descriptor pix_pt_t;

enum { PROC_DEAD, PROC_RUNNABLE} ;

typedef struct pix_env {
    // register save area: keep it at offset 0 for easy
    // assembly calculations.
    uint32_t reg_save[16 + 1];  // 16 general purpose registers + spsr

    // process id: i think we need a virtual one as well to make user 
    // checksums work.
    uint32_t pid;

    // address space id.  for now we keep it simple by killing off any asid on
    // exit.  can be more clever later.
    uint8_t asid;

    pix_pt_t *pt;       // page table.

    // used by the equiv code.
    pix_equiv_t ctx;

    uint32_t state; // free or runnable.
} pix_env_t;


// create a new pix structure that fills in the 
// register save area with the right initial values so 
// that when you use <switch_to> on it for the first time
// it will behave identically to calling <user_mode_run_fn>
pix_env_t pix_env_mk(uint32_t pc, uint32_t sp);

// global variable that always points to the currently running
// process.
// 
// NOTE: this is the slow way to do things: can instead switch 
// to using a coprocessor register.
extern pix_env_t *pix_cur_process;

// change these to use coproc register.
static inline pix_env_t *pix_get_curproc(void) 
    { return pix_cur_process; }   
static inline void pix_set_curproc(pix_env_t *c) 
    { pix_cur_process = c; }


// low level code to switch processes: load the values in 
// <regs> (r0-r15, spsr) and jump to the result.
// 
// XXX: you implement this.
void switchto_asm(uint32_t *regs) __attribute__((noreturn));


/*****************************************************************
 * for extensions: let me know if you get here.
 */

// switch to the next runnable process.
void schedule(void) __attribute__((noreturn));

// clone the page table <pt_old>: copy any global mappings over,
// duplicate (allocate + copy) any non-global pages.
void vm_pt_clone(pix_pt_t *pt_new,  const pix_pt_t *pt_old);

/*****************************************************************
 * 1MB section alloc/free routines.
 */

// free section <secn>
unsigned sec_free(uint32_t secn);
// allocate section <secn>: error if already allocated.
long sec_alloc_exact(uint32_t secn);
// allocate any free 1MB section
long sec_alloc(void);
// extract the section number from <pa> and allocate it.
unsigned sec_alloc_pa(uint32_t pa);
// compute the physical address 
uint32_t sec_to_pa(uint32_t secn);


#endif
