#ifdef RPI_FP_ENABLED

.globl enable_fpu
enable_fpu:
    @  enable fpu
    mrc p15, 0, r0, c1, c0, 2
    orr r0,r0,#0x300000 ;@ single precision
    orr r0,r0,#0xC00000 ;@ double precision
    mcr p15, 0, r0, c1, c0, 2
    mov r0,#0x40000000
    fmxr fpexc,r0

    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @ trying this from dwelch.
    ;@ (PSR_UND_MODE|PSR_FIQ_DIS|PSR_IRQ_DIS)
    mov r0,#0xDB
    msr cpsr_c,r0
    mov sp,#0x00100000

    ;@ (PSR_SVC_MODE|PSR_FIQ_DIS|PSR_IRQ_DIS)
    mov r0,#0xD3
    msr cpsr_c,r0
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    bx lr


.globl m4add
m4add:
    vmov s0,r0
    vmov s1,r1
    vmov s2,r2
    vmov s3,r3
    vadd.f32 s4,s0,s1
    vadd.f32 s5,s2,s3
    vmov r0,s5
    bx lr

.globl m4add2
m4add2:
    vmov s0,r0
    vmov s1,r1
    vmov s2,r2
    vmov s3,r3
    vadd.f32 s4,s0,s1
    vadd.f32 s5,s2,s3
    vmov r0,s4
    bx lr

.globl m4vmov
m4vmov:
    vmov s4,r0
    vmov s5,r0
    bx lr

.globl fp_is_enabled
fp_is_enabled: .word 1

#endif 
