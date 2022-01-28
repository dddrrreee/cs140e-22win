#include "lab-asm-macros.h"

MK_FN(check_push_asm)

    mov r1, sp      @ before
    push {r0}
    mov r0, sp      @ after
    bl after_push
    NOTREACHED

