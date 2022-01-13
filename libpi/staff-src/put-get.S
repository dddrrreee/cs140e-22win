@ dwelch routines for reading/writing memory.  we shoud be able to merge
@ the put32/PUT32's.  (we have different versions b/c of different prototypes)


@ assembler kills off back-to-back labels?

@ we put these in start.S so that they are in the same location in everyone's
@ binary --- we had seperated them out so that you could override them by dropping
@ in different .o's, but never wound up doing that, so pulling it out.
@.globl put32
@put32:
@    str r1,[r0]
@    bx lr
@
@.globl PUT32
@PUT32:
@    str r1,[r0]
@    bx lr




.globl put16
put16:
    strh r1,[r0]
    bx lr

.globl PUT16
PUT16:
    strh r1,[r0]
    bx lr


.globl put8
put8:
    strb r1,[r0]
    bx lr

.globl PUT8
PUT8:
    strb r1,[r0]
    bx lr



@.globl get32
@get32:
@    ldr r0,[r0]
@    bx lr
@
@@.globl GET32
GET32:
@    ldr r0,[r0]
@    bx lr
@
.globl GETPC
GETPC:
    mov r0,lr
    bx lr

.globl dummy
dummy:
    bx lr
