@ hacked code for testing vector base code and testing the
@ difference between the position-independent "generic" method
@ of last lab (that used the ldr instruction) with the faster
@ method of a relative branch (so: no memory load, no cache miss,
@ also, the ldr will commonly = a misprediction).
#include "rpi-asm.h"

MK_FN(bad_exception_asm)
    mov sp, #INT_STACK_ADDR
    mov r0, lr
    b asm_not_reached_helper

@ our trivial, single system call that adds 1 to
@ r0 and returns.
@
@ this is about the minimum overhead for a call we can
@ have since there are no saving/restoring of registers
@ or additional jumps.
@
@ note we will trash super's lr
MK_FN(swi_trivial_vector)
    add r0, r0, #1
    movs pc, lr

@ initiate our trivial system call: we have to save lr b/c the
@ hardware will trash it (since we are at SUPER level, which
@ is where the swi exception will run).
MK_FN(swi_add)
    push {lr}
    swi 1
    pop {lr}
    bx lr


@ for use with 3-122 setting base vector.
@
@ One issue is that you will have to flush the data / instruction
@ cache when you modify this.
@ may want to go to a slower one if you want to override: load
@ from a table.

@ we use the original <ldr> method to jump to an exception vector.
.align 5
.globl _interrupt_vector_slow
_interrupt_vector_slow:
  ldr pc, =bad_exception_asm
  ldr pc, =bad_exception_asm
  ldr pc, =swi_trivial_vector
  ldr pc, =bad_exception_asm
  ldr pc, =bad_exception_asm
  ldr pc, =bad_exception_asm
  ldr pc, =bad_exception_asm

@ this uses a relative branch <b> --- we can do this since we don't
@ have to copy the vector anywhere.
.align 5
.globl _interrupt_vector_fast
_interrupt_vector_fast:
  ldr pc, =bad_exception_asm
  ldr pc, =bad_exception_asm
  @ very cool.
  b swi_trivial_vector
  ldr pc, =bad_exception_asm
  ldr pc, =bad_exception_asm
  ldr pc, =bad_exception_asm
  ldr pc, =bad_exception_asm
