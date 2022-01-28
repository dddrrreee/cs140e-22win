#include "lab-asm-macros.h"

@ save just the callee and lr.
@ before doing so, write the number of
@ each reg to it.  e.g.:
@     mov r4, #4
@ this lets us check that things are 
@ at the right offset.
@
@ then:
@   - push all the registers
@   - store the sp to the address in r0.
@   - call <after_writing_regs>
MK_FN(write_regs_to_stack)
    mov r4, #4
    mov r5, #5
    mov r6, #6
    mov r7, #7
    mov r8, #8
    mov r9, #9
    mov r10, #10
    mov r11, #11
    mov lr, #14
    push {r4,r5,r6,r7,r8,r9,r10,r11,lr}
    str sp, [r0,#0]
    mov r0, sp
    bl after_writing_regs
    NOTREACHED
