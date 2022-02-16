@ stripped down start for testing.
#include "rpi-asm.h"

// To keep this in the first portion of the binary.
.section ".text.boot"

.globl _start
_start:
    @ assume we already have stack setup
    @mov sp, #LIBOS_STACK
    @ sleazy: not setting up the the bss.
    bl notmain
    mov r0, #-1
    b sys_exit 
