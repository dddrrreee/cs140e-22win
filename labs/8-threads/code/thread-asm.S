/*
 * implement these.  ordered in difficulty.
 */
#include "rpi-asm.h"

#define NOTREACHED mov r0, pc; bl asm_not_reached
#define UNIMPL mov r0, pc; bl asm_not_implemented

@ return the current stack pointer.
MK_FN(rpi_get_sp)
    UNIMPL
	bx lr

MK_FN(rpi_cswitch)
    UNIMPL
    bx lr

@ [Make sure you can answer: why do we need to do this?]
@
@ use this to setup each thread for the first time.
@ setup the stack so that when cswitch runs it will:
@	- load address of <rpi_init_trampoline> into LR
@	- <code> into r1, 
@	- <arg> into r0
@ 
MK_FN(rpi_init_trampoline)
    UNIMPL
