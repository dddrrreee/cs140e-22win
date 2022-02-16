#include "rpi-asm.h"

@ just use the default int stack address.
#define INT_STACK INT_STACK_ADDR

#define not_reached()  mov r0, pc; b asm_not_reached

@ create an interrupt vector named <name>
@ user has to define the different trampolines.
#define MK_INT_VEC(name)    \
    .align 5;               \
    .globl name;            \
    name:                   \
        b reset;            \
        b undef;            \
        b swi_handler;      \
        b prefetch_abort;   \
        b data_abort;       \
        b reset;            \
        b interrupt

@ save all the stuff we need, then call the given routine.
#define TRAMPOLINE(fn)              \
  mov sp, #INT_STACK;               \
  push  {r0-r12,lr};                \
  mov   r0, lr;                     \
  bl    fn;                         \
  pop   {r0-r12,lr};                \
  movs    pc, lr


// keep the name of the trampoline private.
#define MK_TRAMPOLINE(name, offset, fn)  \
    name: sub   lr, lr, # (offset); TRAMPOLINE(fn)


@ call the default handlers and blow up.  override if you
@ want to do something fancier.
MK_TRAMPOLINE(reset,            4, reset_vector)
MK_TRAMPOLINE(undef,            4, undefined_instruction_vector)
MK_TRAMPOLINE(prefetch_abort,   4, prefetch_abort_vector)
MK_TRAMPOLINE(data_abort,       8, data_abort_vector)
MK_TRAMPOLINE(interrupt,        4, int_vector)


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ part0/part1: just handle system calls.
@

swi_handler:
    @ when we do equiv, need to make sure we restore everything back.
    @ we over-save/restore and trim after equiv.
    push {r1-r12,r14}
    bl do_syscall
    pop {r1-r12,r14}
    movs pc, lr

.align 5; 
.globl part1_swi_vec
part1_swi_vec: 
    b reset
    b undef
    b swi_handler
    b prefetch_abort
    b data_abort
    b reset
    b interrupt


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ part2: handle single stepping
@

prefetch_handler_single_step:
    sub   lr, lr, #4
    TRAMPOLINE(prefetch_abort_single_step)

@ have a single step handler for prefetch abort.
.align 5; 
.globl part2_single_step_vec
part2_single_step_vec: 
    b reset
    b undef
    b swi_handler
    b prefetch_handler_single_step
    b data_abort
    b reset
    b interrupt



@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ part3: do equivalance for pc

@ setup and call prefetch_abort_equiv_pc (look at the single step)
prefetch_handler_equiv_pc:
    asm_not_implemented()

@ have a single step handler for prefetch abort.
@ have a equiv handler for prefetch abort.
.align 5; 
.globl part3_equiv_pc_vec
part3_equiv_pc_vec: 
    b reset
    b undef
    b swi_handler
    b prefetch_handler_equiv_pc
    b data_abort
    b reset
    b interrupt


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ part4: do equivalance for all regs
@   push all the regs and spsr onto the stack and pass it to the handler.


@ setup and call prefetch_abort_equiv
prefetch_handler_equiv:
    asm_not_implemented()

@ have a single step handler for prefetch abort.
@ have a equiv handler for prefetch abort.
.align 5; 
.globl part4_equiv_vec
part4_equiv_vec: 
    b reset
    b undef
    b swi_handler
    b prefetch_handler_equiv
    b data_abort
    b reset
    b interrupt


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ before jumping to user:
@  - clear all bits in cpsr besides mode
@  - clear all registers before jumping to user space besides sp
@
@ does not return.
MK_FN(user_mode_run_fn)
    @ use cps to switch to user level.
    @ make sure to do a "prefetch flush" to 
    @ flush the state  (see rpi-asm.h)
    @
    @ for later, you should clear all the registers.
    asm_not_implemented()
