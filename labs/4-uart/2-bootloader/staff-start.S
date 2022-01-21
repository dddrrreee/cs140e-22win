@ based on dwelch's start for bootloader.

// To keep this in the first portion of the binary.
.section ".text.boot"


.globl _start
_start:
    b skip
.space 0x200000-0x8004,0
skip:
    mov sp,#0x08000000
    bl notmain
hang: b rpi_reboot

@ cannot overwrite this code or anything after it --- make sure
@ sure you check  that the code sent to the bootloader
@ does not collide with the address of <BRANCHTO>
.globl BRANCHTO
BRANCHTO:
    bx r0


@ keep these here so that they are in the same location in every binary.
@ if you want to override them easily, can re-define the name in rpi-asm.h
.globl put32
.globl PUT32
put32:
PUT32:
    str r1,[r0]
    bx lr

.globl get32
.globl GET32

get32:
GET32:
    ldr r0,[r0]
    bx lr

