// helper code for the lab: typically small routines useful for figuring
// out some assembly fact
#include "rpi.h"

// called from asm
void asm_not_implemented(uint32_t pc) {
    panic("routine at roughly pc=%p: not implemented\n", pc);
}
// called from asm
void asm_not_reached(uint32_t pc) {
    panic("impossible: returned to %p\n", pc);
}
