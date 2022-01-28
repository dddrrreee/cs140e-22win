#include "rpi-asm.h"

#define NOTREACHED mov r0, pc; bl asm_not_reached
#define UNIMPL mov r0, pc; bl asm_not_implemented
